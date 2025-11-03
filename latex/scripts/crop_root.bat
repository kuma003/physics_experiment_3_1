@echo off
REM crop_root.bat
REM Usage: crop_root.bat [target_dir]
REM Default target_dir: figs\root (relative to current working directory)

REM --- set target directory ---
if "%~1"=="" (
  set "TARGET=figs\root"
) else (
  set "TARGET=%~1"
)

necho Cropping PDFs in "%TARGET%" ...

n=0
for %%F in ("%TARGET%\*.pdf") do (
  echo.
  echo Processing: %%~fF
  REM create cropped temporary file next to original
  pdfcrop "%%~fF" "%%~dpnF-crop.pdf"
  if errorlevel 1 (
    echo ERROR: pdfcrop failed for %%~fF
  ) else (
    REM overwrite original with cropped output
    move /Y "%%~dpnF-crop.pdf" "%%~fF" >nul 2>&1
    if errorlevel 1 (
      echo ERROR: move failed for %%~fF
    ) else (
      echo Overwrote %%~nxF
    )
  )
)
echo.
echo All done.
pause
