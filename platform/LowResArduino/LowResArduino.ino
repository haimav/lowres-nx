#include <Adafruit_Arcada.h>
//#include <Adafruit_ZeroDMA.h>

Adafruit_Arcada arcada;

struct Machine;
struct MachineInternals;
struct Interpreter;
struct DiskDrive;
struct Overlay;
struct DataManager;

enum KeyboardMode {
    KeyboardModeOff,
    KeyboardModeOn,
    KeyboardModeOptional,
    KeyboardModeForceSize = 0xFFFFFFFF
};

struct ControlsInfo {
    enum KeyboardMode keyboardMode;
    int numGamepadsEnabled;
    bool isTouchEnabled;
    bool isAudioEnabled;
};

enum ErrorCode {
    ErrorNone,
    
    ErrorCouldNotOpenProgram,
    ErrorTooManyTokens,
    ErrorRomIsFull,
    ErrorIndexAlreadyDefined,
    ErrorUnterminatedString,
    ErrorUnexpectedCharacter,
    ErrorReservedKeyword,
    ErrorSyntax,
    ErrorSymbolNameTooLong,
    ErrorTooManySymbols,
    ErrorTypeMismatch,
    ErrorOutOfMemory,
    ErrorElseWithoutIf,
    ErrorEndIfWithoutIf,
    ErrorExpectedCommand,
    ErrorNextWithoutFor,
    ErrorLoopWithoutDo,
    ErrorUntilWithoutRepeat,
    ErrorWendWithoutWhile,
    ErrorLabelAlreadyDefined,
    ErrorTooManyLabels,
    ErrorExpectedLabel,
    ErrorUndefinedLabel,
    ErrorArrayNotDimensionized,
    ErrorArrayAlreadyDimensionized,
    ErrorVariableAlreadyUsed,
    ErrorIndexOutOfBounds,
    ErrorWrongNumberOfDimensions,
    ErrorInvalidParameter,
    ErrorReturnWithoutGosub,
    ErrorStackOverflow,
    ErrorOutOfData,
    ErrorIllegalMemoryAccess,
    ErrorTooManyCPUCyclesInInterrupt,
    ErrorNotAllowedInInterrupt,
    ErrorIfWithoutEndIf,
    ErrorForWithoutNext,
    ErrorDoWithoutLoop,
    ErrorRepeatWithoutUntil,
    ErrorWhileWithoutWend,
    ErrorDirectoryNotLoaded,
    ErrorDivisionByZero,
    ErrorVariableNotInitialized,
    ErrorArrayVariableWithoutIndex,
    ErrorEndSubWithoutSub,
    ErrorSubWithoutEndSub,
    ErrorSubCannotBeNested,
    ErrorUndefinedSubprogram,
    ErrorExpectedSubprogramName,
    ErrorArgumentCountMismatch,
    ErrorSubAlreadyDefined,
    ErrorTooManySubprograms,
    ErrorSharedOutsideOfASubprogram,
    ErrorGlobalInsideOfASubprogram,
    ErrorExitSubOutsideOfASubprogram,
    ErrorKeyboardNotEnabled,
    ErrorAutomaticPauseNotDisabled,
    ErrorGamepadNotEnabled,
    ErrorTouchNotEnabled,
    ErrorInputChangeNotAllowed,
    ErrorCodeForceSize = 0xFFFFFFFF
};


struct CoreError {
    ErrorCode code;
    int sourcePosition;
};

struct CoreDelegate {
    void *context;
    
    /** Called on error */
    void (*interpreterDidFail)(void *context,  CoreError coreError);
    
    /** Returns true if the disk is ready, false if not. In case of not, core_diskLoaded must be called when ready. */
    bool (*diskDriveWillAccess)(void *context,  DataManager *diskDataManager);
    
    /** Called when a disk data entry was saved */
    void (*diskDriveDidSave)(void *context,  DataManager *diskDataManager);
    
    /** Called when keyboard or gamepad settings changed */
    void (*controlsDidChange)(void *context,  ControlsInfo controlsInfo);
    
    /** Called when persistent RAM will be accessed the first time */
    void (*persistentRamWillAccess)(void *context, uint8_t *destination, int size);
    
    /** Called when persistent RAM should be saved */
    void (*persistentRamDidChange)(void *context, uint8_t *data, int size);
};

struct Core {
    Machine *machine;
    MachineInternals *machineInternals;
    Interpreter *interpreter;
    DiskDrive *diskDrive;
    Overlay *overlay;
    CoreDelegate *delegate;
};

extern "C" {
  void core_init(Core *core);
  CoreError core_compileProgramEx(Core *core, const char *sourceCode, int inplaceSource);
  void core_willRunProgram(Core *core, long secondsSincePowerOn);

  void interpreterDidFail(void *context,  CoreError coreError) {
    Serial.println("interpreterDidFail");
  }
    
  /** Returns true if the disk is ready, false if not. In case of not, core_diskLoaded must be called when ready. */
  bool diskDriveWillAccess(void *context,  DataManager *diskDataManager) {
    Serial.println("diskDriveWillAccess");
  }
    
  /** Called when a disk data entry was saved */
  void diskDriveDidSave(void *context,  DataManager *diskDataManager) {
    Serial.println("diskDriveDidSave");
  }
    
  /** Called when keyboard or gamepad settings changed */
  void controlsDidChange(void *context,  ControlsInfo controlsInfo) {
    Serial.println("controlsDidChange");
  }
    
  /** Called when persistent RAM will be accessed the first time */
  void persistentRamWillAccess(void *context, uint8_t *destination, int size) {
    Serial.println("persistentRamWillAccess");
  }
    
  /** Called when persistent RAM should be saved */
  void persistentRamDidChange(void *context, uint8_t *data, int size) {
    Serial.println("persistentRamDidChange");
  }

  void cdbg_print(char *l) {
    Serial.print(l);
  }
}

Core core;

void setup() {
  Serial.begin(115200);
  
  arcada.begin();
  arcada.filesysBeginMSD();

  delay(1000);

  //// Init screen with blue so we know its working
  arcada.displayBegin();
  arcada.fillScreen(ARCADA_BLUE);
  arcada.setBacklight(255);
  
  //// Set up filesystem
  if (arcada.filesysBegin()) 
    Serial.println("Found filesystem!");
  else 
    arcada.haltBox("No filesystem found! For QSPI flash, load CircuitPython. For SD cards, format with FAT");
  

  //// Loading file - currently to memory. TODO: to FLASH.
  arcada.fillScreen(ARCADA_CYAN);
  arcada.infoBox("Loading run.nx into chip memory...", 0);
  File file = arcada.open("/run.nx", O_READ);
  int file_size = file.fileSize();
  Serial.printf("run.nx file size is %d\n", file_size);
  char *nxfile = (char *)malloc(file_size + 1);
  file.read(nxfile, file_size);
  nxfile[file_size] = 0;  // put terminator
  file.close();

  arcada.fillScreen(ARCADA_CYAN);
  arcada.infoBox("Compiling program...", 0); 
  core_init(&core);
  core.delegate = (CoreDelegate *)calloc(1, sizeof(CoreDelegate));
  core.delegate->interpreterDidFail = interpreterDidFail;
  core.delegate->diskDriveWillAccess = diskDriveWillAccess;
  core.delegate->diskDriveDidSave = diskDriveDidSave;
  core.delegate->controlsDidChange = controlsDidChange;
  core.delegate->persistentRamWillAccess = persistentRamWillAccess;
  core.delegate->persistentRamDidChange = persistentRamDidChange;
  
  CoreError err = core_compileProgramEx(&core, nxfile, 1);
  //Serial.printf("code is %d, at position %d\n", err.code, err.sourcePosition);
  if (err.code == ErrorNone)
    Serial.println("Compiliation successful!");
  else 
    Serial.printf("code is %d, at position %d\n", err.code, err.sourcePosition);
  core_willRunProgram(&core, 0);
   
}

bool bl = false;
void loop() {
  if (bl)
    arcada.fillScreen(ARCADA_BLUE);
  else 
    arcada.fillScreen(ARCADA_BLACK);
  bl = !bl;
  delay(500);
}
