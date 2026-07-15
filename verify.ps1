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
    - Format Check: clang-format --dry-run --Werror를 src/ 아래 실제 프로젝트 소스에만
      적용한다(packages/, x64/, .vs/ 등은 제외). 검사 전용이며 자동 수정하지 않는다.
#>

[CmdletBinding()]
param(
    [string]$Configuration = "Debug",
    [string]$Platform = "x64"
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
    $cmd = Get-Command clang-format.exe -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
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
$sourceFiles = Get-ChildItem -Path (Join-Path $RepoRoot "src") -Recurse -Include *.cpp, *.h, *.hpp
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
