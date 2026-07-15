#include <gtest/gtest.h>

#define NOMINMAX
#include <Windows.h>

#include <iostream>

#include "ApproveOrderUseCase.h"
#include "ConsoleInputParsing.h"
#include "ConsoleOrderApprovalView.h"
#include "ConsoleOrderView.h"
#include "ConsoleView.h"
#include "ListReservedOrdersUseCase.h"
#include "ListSamplesUseCase.h"
#include "OrderApprovalController.h"
#include "OrderController.h"
#include "OrderRepository.h"
#include "ProductionQueueRepository.h"
#include "RegisterSampleUseCase.h"
#include "RejectOrderUseCase.h"
#include "ReserveOrderUseCase.h"
#include "SampleController.h"
#include "SampleRepository.h"
#include "SearchSampleUseCase.h"

namespace {
const char kDataPath[] = "data/sample.json";

void ShowMainMenu() {
    std::cout << "\n=== 반도체 시료 생산주문관리 시스템 ===\n"
              << "[1] 시료 관리\n"
              << "[2] 시료 주문\n"
              << "[3] 주문 승인/거절\n"
              << "[0] 종료\n"
              << "선택 > ";
}
}  // namespace

int main(int argc, char** argv) {
    // 소스는 /utf-8로 컴파일되어 한글 리터럴이 UTF-8 바이트로 실행 파일에 들어간다. 콘솔의 기본
    // 코드페이지(한국어 Windows에서는 CP949)와 맞지 않으면 출력이 깨지므로, 콘솔 입출력
    // 코드페이지를 UTF-8(65001)로 맞춘다.
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // 인자가 하나라도 있으면 테스트 모드로 취급한다(DataPersistence-minji-0559와 동일한 규칙).
    // Visual Studio의 GoogleTest 어댑터는 --gtest_list_tests 같은 자체 플래그로 실행 파일을
    // 호출하므로, 특정 플래그 이름 대신 "인자 존재 여부"로 분기해야 어댑터와도 정상 동작한다.
    if (argc > 1) {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }

    using namespace SampleOrderSystem;

    SampleRepository sampleRepository(kDataPath);
    OrderRepository orderRepository(kDataPath);  // Sample과 같은 공유 문서(PRD 5.5.5)
    ProductionQueueRepository productionQueueRepository(kDataPath);  // 위와 동일 문서 공유

    RegisterSampleUseCase registerUseCase(sampleRepository);
    ListSamplesUseCase listUseCase(sampleRepository);
    SearchSampleUseCase searchUseCase(sampleRepository);
    ReserveOrderUseCase reserveUseCase(sampleRepository, orderRepository);
    ListReservedOrdersUseCase listReservedUseCase(orderRepository);
    ApproveOrderUseCase approveUseCase(orderRepository, sampleRepository,
                                       productionQueueRepository);
    RejectOrderUseCase rejectUseCase(orderRepository);

    ConsoleView sampleView;
    SampleController sampleController(sampleView, registerUseCase, listUseCase, searchUseCase);

    ConsoleOrderView orderView;
    OrderController orderController(orderView, reserveUseCase);

    ConsoleOrderApprovalView orderApprovalView;
    OrderApprovalController orderApprovalController(orderApprovalView, listReservedUseCase,
                                                    approveUseCase, rejectUseCase);

    // 아직 구현되지 않은 메인 메뉴 항목(모니터링, 생산 라인, 출고 처리)은 해당 Phase가 끝날 때마다
    // 하나씩 여기에 채워 넣는다(Phase 0에서 결정한 방식).
    bool running = true;
    while (running) {
        ShowMainMenu();
        std::string line;
        if (!std::getline(std::cin, line)) {
            break;  // EOF: 무한 루프 없이 종료
        }
        switch (ParseMenuChoice(line).value_or(-1)) {
            case 1:
                sampleController.Run();
                break;
            case 2:
                orderController.Run();
                break;
            case 3:
                orderApprovalController.Run();
                break;
            case 0:
                running = false;
                break;
            default:
                break;
        }
    }

    return 0;
}
