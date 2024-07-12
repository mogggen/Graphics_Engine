Remove-Item build -Recurse -Force;
cmake -S . -B build
& build/gscept_lab_env.sln