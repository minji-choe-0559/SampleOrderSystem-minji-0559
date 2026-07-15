#pragma once

#include "SampleRecord.h"
#include "SampleViewModel.h"

namespace SampleOrderSystem {

// ListSamplesUseCase와 SearchSampleUseCase가 공유하는 Domain→Presentation 매핑
// (code-reviewer 지적: 중복 정의 제거).
inline SampleViewModel ToViewModel(const SampleRecord& record) {
    SampleViewModel viewModel;
    viewModel.sampleCode = record.sampleCode;
    viewModel.name = record.name;
    viewModel.avgProcessTime = record.avgProcessTime;
    viewModel.yieldRate = record.yieldRate;
    viewModel.stock = record.stock;
    return viewModel;
}

}  // namespace SampleOrderSystem
