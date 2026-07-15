#include <gtest/gtest.h>

#define NOMINMAX
#include <Windows.h>

#include "ConsoleView.h"
#include "ListSamplesUseCase.h"
#include "RegisterSampleUseCase.h"
#include "SampleController.h"
#include "SampleRepository.h"
#include "SearchSampleUseCase.h"

namespace {
const char kDataPath[] = "data/sample.json";
}

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

    SampleRepository repository(kDataPath);
    RegisterSampleUseCase registerUseCase(repository);
    ListSamplesUseCase listUseCase(repository);
    SearchSampleUseCase searchUseCase(repository);
    ConsoleView view;
    SampleController controller(view, registerUseCase, listUseCase, searchUseCase);
    controller.Run();

    return 0;
}
