[![Build Status](https://travis-ci.org/cicatrice/travis-test.svg?branch=master)](https://travis-ci.org/cicatrice/travis-test)

# Process Dumper
Dumps entire memory of target process on the user-mode.

# Usage

```
dumper.exe target.exe
```

# Requirements
Only supports win10 x64  
Administrator privilege required for run.

# Information

This process might be recognized as malware since it has a handle of the target process.  
Additionaly, this may not be work for the process that belongs to the os-core, and protected by the 3rd party protectors, especially hiding handle by `ObRegisterCallbacks()`
