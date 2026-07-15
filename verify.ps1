<#
.SYNOPSIS
    SampleOrderSystem 단일 verify 명령.
    Build, Unit Test, Compiler Warning, Format Check를 한 번에 검증한다.
    CLAUDE.md "Harness" 절, PLAN.md Phase 0 참고.

.DESCRIPTION
    - Build: MSBuild로 SampleOrderSystem-minji-0559.slnx를 빌드한다(Debug|x64).
      Warning 정책(/W4, /WX)과 /utf-8은 Directory.Build.props가 전체 프로젝트에
      공통 적용한다. 외부 벤더(GoogleTest/GoogleMock) 헤더는 TreatAngleIncludeAsExternal
      + ExternalWarningLevel로 경고-에러 대상에서 제외한다.
    - Unit Test: 테스트 프로젝트가 아직 없으면(Phase 0) 이 단계를 건너뛰고 그 사실을
      출력한다. 테스트 실행 파일이 있으면 실행하고 실패 시 전체를 실패시킨다.
    - Compiler Warning: Build 단계에서 이미 /W4 /WX로 검증되므로 별도 실행 없이 결과만
      알린다.
    - Format Check: clang-format --dry-run --Werror를 적용한다(packages/, x64/, .vs/ 등은
      제외). 검사 전용이며 자동 수정하지 않는다. 기본값은 git으로 변경 감지된 src/ 파일
      (`origin/master`와의 병합 기준점 대비 diff + staged + 추적 안 되는 신규 파일)만
      검사한다(Phase가 늘어날수록 이미 검증된 예전 파일을 매번 재검사하는 낭비를 줄이기
      위함). git 자체를 쓸 수 없는 환경(저장소 아님, git 미설치 등)에서만 전체 모드로
      폴백한다 — **git은 쓸 수 있는데 변경된 파일이 0개인 경우는 폴백 대상이 아니며, 그때는
      검사 대상 0개로 정상 통과 처리한다.**
      -FullFormatCheck 스위치를 주면 항상 src/ 전체를 강제로 검사한다. PLAN.md Phase
      6(통합 리팩터링과 최종 Review)의 완료 조건("단일 verify 명령 전체 통과")과 신규 클론
      직후 최초 확인은 반드시 이 스위치로 실행한다.
#>

[CmdletBinding()]
param(
    [string]$Configuration = "Debug",
    [string]$Platform = "x64",
    [switch]$FullFormatCheck
)

$ErrorActionPreference = "Stop"
$RepoRoot = $PSScriptRoot

function Find-MSBuild {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    if (Test-Path $vswhere) {
        $path = & $vswhere -latest -products * -requires Microsoft.Component.MSBuild `
            -find MSBuild\**\Bin\MSBuild.exe | Select-Object -First 1
        if ($path) { return $path }
    }
    $cmd = Get-Command msbuild.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    throw "MSBuild.exe를 찾을 수 없습니다. Visual Studio Build Tools 설치를 확인하세요."
}

function Find-ClangFormat {
    # 1) PATH에 있으면 그대로 사용(가장 빠름).
    $cmd = Get-Command clang-format.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }

    # 2) VS의 알려진 고정 설치 경로 패턴만 확인(재귀 없이, ~30ms 수준). clang-format.exe가
    #    PATH에 없는 게 보통이라 verify.ps1을 실행할 때마다 이 경로를 타는데, -Recurse 전체
    #    검색(~300ms)보다 이쪽이 매번 반복해도 훨씬 저렴하다.
    $known = Get-ChildItem -Path "${env:ProgramFiles}\Microsoft Visual Studio\*\*\VC\Tools\Llvm\x64\bin\clang-format.exe" `
        -ErrorAction SilentlyContinue
    if ($known) { return ($known | Sort-Object FullName | Select-Object -First 1).FullName }

    # 3) 비표준 설치 위치 대비 최후 폴백(느린 전체 재귀 검색).
    $candidates = Get-ChildItem "${env:ProgramFiles}\Microsoft Visual Studio" `
        -Recurse -Filter "clang-format.exe" -ErrorAction SilentlyContinue |
        Where-Object { $_.FullName -match '\\x64\\' } |
        Sort-Object FullName
    if ($candidates) { return $candidates[0].FullName }
    throw "clang-format.exe를 찾을 수 없습니다. Visual Studio의 C++ Clang 도구 구성 요소를 설치하세요."
}

Write-Output "==== [1/4] Build ($Configuration|$Platform) ===="
$msbuild = Find-MSBuild
& $msbuild "$RepoRoot\SampleOrderSystem-minji-0559.slnx" `
    "/p:Configuration=$Configuration" "/p:Platform=$Platform" /nologo /v:minimal
if ($LASTEXITCODE -ne 0) { throw "Build 실패 (exit $LASTEXITCODE). Warning은 /W4 /WX로 에러 처리됨." }
Write-Output "Build 성공 (Compiler Warning 검증 포함: /W4, /WX, 외부 벤더 헤더 제외)"

Write-Output ""
Write-Output "==== [2/4] Unit Test ===="
# DataPersistence-minji-0559와 동일한 규칙: 별도 Test 실행 파일을 만들지 않고, 같은 exe를
# 인자와 함께 호출하면 main()이 GoogleTest 러너로 분기한다(인자 없이 호출하면 콘솔 앱 실행).
$appExe = Join-Path $RepoRoot "$Platform\$Configuration\SampleOrderSystem-minji-0559.exe"
if (-not (Test-Path $appExe)) {
    throw "실행 파일을 찾을 수 없습니다: $appExe"
}
& $appExe --gtest_color=no
if ($LASTEXITCODE -ne 0) { throw "Unit Test 실패 (exit $LASTEXITCODE)" }
Write-Output "Unit Test 통과"

Write-Output ""
Write-Output "==== [3/4] Compiler Warning ===="
Write-Output "Build 단계에서 /W4 /WX로 이미 확인됨 (프로젝트 코드 기준, 외부 벤더 헤더 제외)"

Write-Output ""
Write-Output "==== [4/4] Format Check (clang-format, 검사 전용) ===="
$clangFormat = Find-ClangFormat

function Get-ChangedSourceFiles {
    # git을 쓸 수 없으면 $null을 반환해 호출자가 전체 스캔으로 폴백하게 한다.
    # 스크립트 전역의 $ErrorActionPreference = "Stop"이 git(네이티브 커맨드)의 stderr 경고
    # (예: "LF will be replaced by CRLF")까지 치명적 오류로 격상시켜 verify.ps1 전체가
    # 죽는 문제가 있었다 — 이 함수 안에서만 지역적으로 완화한다(함수 스코프라 전역에는
    # 영향 없음).
    $ErrorActionPreference = "Continue"
    try {
        Push-Location $RepoRoot
        git rev-parse --is-inside-work-tree *> $null
        if ($LASTEXITCODE -ne 0) { return $null }

        $mergeBase = git merge-base HEAD origin/master 2>$null
        if ($LASTEXITCODE -ne 0 -or -not $mergeBase) { $mergeBase = "HEAD" }

        $diffed = git diff --name-only $mergeBase -- src 2>$null
        $staged = git diff --name-only --cached -- src 2>$null
        $untracked = git ls-files --others --exclude-standard -- src 2>$null

        $paths = @($diffed) + @($staged) + @($untracked) |
            Where-Object { $_ -match '\.(cpp|h|hpp)$' } |
            Sort-Object -Unique
        return , @($paths | ForEach-Object { Join-Path $RepoRoot $_ } | Where-Object { Test-Path $_ })
    } finally {
        Pop-Location
    }
}

$sourceFiles = $null
$scopedMode = $false
if (-not $FullFormatCheck) {
    $changedPaths = Get-ChangedSourceFiles
    if ($null -ne $changedPaths) {
        $scopedMode = $true
        $sourceFiles = $changedPaths | Get-Item -ErrorAction SilentlyContinue
    }
}
if (-not $scopedMode) {
    $sourceFiles = Get-ChildItem -Path (Join-Path $RepoRoot "src") -Recurse -Include *.cpp, *.h, *.hpp
    Write-Output "전체 검사 모드: src/ 아래 $($sourceFiles.Count)개 파일 검사"
} else {
    Write-Output "변경 감지 모드: git 기준 $($sourceFiles.Count)개 파일만 검사(-FullFormatCheck로 전체 검사 가능)"
}

$formatFailed = $false
foreach ($file in $sourceFiles) {
    & $clangFormat --style=file --dry-run --Werror $file.FullName
    if ($LASTEXITCODE -ne 0) {
        Write-Output "  FORMAT 위반: $($file.FullName)"
        $formatFailed = $true
    }
}
if ($formatFailed) { throw "Format Check 실패. clang-format --style=file -i <file>로 직접 수정 후 재실행하세요." }
Write-Output "Format Check 통과 ($($sourceFiles.Count)개 파일 검사, 자동 수정 없음)"

Write-Output ""
Write-Output "==== 전체 Harness 통과: Build / Unit Test / Compiler Warning / Format Check ===="
exit 0
