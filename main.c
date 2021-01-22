
#include <psp2/kernel/modulemgr.h>
#include <psp2/kernel/clib.h>
#include <psp2/appmgr.h>
#include <psp2/fios2.h>
#include <taihen.h>

#include <PERSONAL/userland/taihen.h>
#include <PERSONAL/userland/log.h>

static SceFiosOverlayID overlayID = 0;

static char* srcPath = "invalid:";

#define RC1_ALIAS_PATH "ux0:data/RC1"
#define RC2_ALIAS_PATH "ux0:data/RC2"
#define RC3_ALIAS_PATH "ux0:data/RC3"
#define RCC_ALIAS_PATH "ux0:data/RCC"

#define RC1_PAL_IDX 0x180E6
#define RC2_PAL_IDX 0x17116
#define RC3_PAL_IDX 0x17FF6
#define RCC_PAL_IDX 0x1318A

//TODO : Find those offsets to enable US Collection support.
#define RC1_US_IDX 0xFFFFFFFF
#define RC2_US_IDX 0xFFFFFFFF
#define RC3_US_IDX 0xFFFFFFFF
#define RCC_US_IDX 0xFFFFFFFF

DeclareHook(int, FluxFOpen, char * pFilePath, char * Mode, bool Async){
    int fd = TAI_CONTINUE(int, FluxFOpen_hookref, pFilePath, Mode, Async);
    if (Mode && *Mode == 'p'){
        LOG(ANSI_MAGENTA "FluxFOpen(%s, %s, %u) called !\n"ANSI_RESET, pFilePath, Mode, Async);
        LOG("Installing overlay...");
        SceFiosOverlay overlay = {0};
        overlay.type = SCE_FIOS_OVERLAY_TYPE_TRANSLUCENT;
        overlay.order = SCE_FIOS_OVERLAY_ORDER_USER_FIRST;
        sceClibStrncpy(overlay.dst, "/", 5);
        sceClibStrncpy(overlay.src, srcPath, 18);
        int ret = sceFiosOverlayAdd(&overlay, &overlayID);
        LOG("sceRCFiosOverlayAdd returned 0x%08X\n", ret);
        LOG("PSARC is now aliased at " ANSI_CYAN "%s" ANSI_RESET ".\n", srcPath);
        LOG(ANSI_MAGENTA"Returning from hook...\n"ANSI_RESET);
    }
    return fd;
}

void _start() __attribute__((weak, alias("module_start")));
int module_start(SceSize argc, const void* args){
    LOG(ANSI_CYAN "RCFiosOverlay start !\n" ANSI_RESET);
    tai_module_info_t info;
    char titleID[10] = {0};
    char* moduleName = NULL;
    sceAppMgrAppParamGetString(0, 12, titleID, sizeof(titleID));
    sceClibMemset(&info, 0, sizeof(info));
    info.size = sizeof(info);
    int ret = taiGetModuleInfo(moduleName,&info);
    if (ret < 0){
        LOG_ERROR("taiGetModuleInfo(\"%s\") error'ed out (ret = 0x%08X)\n", (moduleName) ? moduleName : "(null)", ret);
        LOG("RCFiosOverlay exiting...\n");
        return SCE_KERNEL_START_FAILED;
    }


    int idx;
    if (sceClibStrcmp(titleID, "PCSF00484") == 0) {
        srcPath = RC1_ALIAS_PATH;
        idx = RC1_PAL_IDX;
    }
    else if (sceClibStrcmp(titleID, "PCSF00485") == 0) {
        srcPath = RC2_ALIAS_PATH;
        idx = RC2_PAL_IDX;
    }
    else if (sceClibStrcmp(titleID, "PCSF00486") == 0) {
        srcPath = RC3_ALIAS_PATH;
        idx = RC3_PAL_IDX;
    }
    else if (sceClibStrcmp(titleID, "PCSF00482") == 0){
        SceKernelModuleInfo modinfo;
        ret = sceKernelGetModuleInfo(info.modid, &modinfo);
        if (ret < 0){
            LOG_ERROR("sceKernelGetModuleInfo() error'ed out (ret = 0x%08X)\n");
            LOG("Staying in backgroud, hoping this is the main menu of the Trilogy...\n");
            return SCE_KERNEL_START_SUCCESS;
        }
        if (sceClibStrstr(modinfo.path, "rc1.self") != NULL){
            srcPath = RC1_ALIAS_PATH;
            idx = RC1_PAL_IDX;
        }
        else if (sceClibStrstr(modinfo.path, "rc2.self") != NULL){
            srcPath = RC2_ALIAS_PATH;
            idx = RC2_PAL_IDX;
        }
        else if (sceClibStrstr(modinfo.path, "rc3.self") != NULL){
            srcPath = RC3_ALIAS_PATH;
            idx = RC3_PAL_IDX;
        }
        else if (  (sceClibStrstr(modinfo.path, "EBOOT.BIN") != NULL) || (sceClibStrstr(modinfo.path, "eboot.bin") != NULL)  ){
            srcPath = RCC_ALIAS_PATH;
            idx = RCC_PAL_IDX;
        }
        else {
            LOG("Couldn't detect a known SELF in path '%s'.\n", modinfo.path);
            LOG("RCFiosOverlay exiting...\n");
            return SCE_KERNEL_START_FAILED;
        }
    }
    else if (sceClibStrcmp(titleID, "PCSA00133") == 0){
        //TODO : find offsets support for US Collection
        //For now, the code will stay stubbed.
        /**
        SceKernelModuleInfo modinfo;
        ret = sceKernelGetModuleInfo(info.modid, &modinfo);
        if (ret < 0){
            LOG_ERROR("sceKernelGetModuleInfo() error'ed out (ret = 0x%08X)\n");
            LOG("Staying in backgroud, hoping this is the main menu of the Collection...\n");
            return SCE_KERNEL_START_SUCCESS;
        }
        if (sceClibStrstr(modinfo.path, "rc1.self") != NULL){
            srcPath = RC1_ALIAS_PATH;
            idx = RC1_US_IDX;
        }
        else if (sceClibStrstr(modinfo.path, "rc2.self") != NULL){
            srcPath = RC2_ALIAS_PATH;
            idx = RC2_US_IDX;
        }
        else if (sceClibStrstr(modinfo.path, "rc3.self") != NULL){
            srcPath = RC3_ALIAS_PATH;
            idx = RC3_US_IDX;
        }
        else if (  (sceClibStrstr(modinfo.path, "EBOOT.BIN") != NULL) || (sceClibStrstr(modinfo.path, "eboot.bin") != NULL)  ){
            LOG("SELF path is '%s'.\n", modinfo.path);
            srcPath = RCC_ALIAS_PATH;
            idx = RCC_US_IDX;
        }
        else {
            LOG("Couldn't detect a known SELF in path '%s'.\n", modinfo.path);
            LOG("RCFiosOverlay exiting...\n");
            return SCE_KERNEL_START_FAILED;
        }
        /**/

        LOG("Sorry, but The Ratchet & Clank Collection US is not supported yet...\n");
        LOG("Exiting...\n");
        return SCE_KERNEL_START_FAILED;
    }
    else {
        LOG("Unknown TitleID %s.\nExiting...\n",titleID);
        return SCE_KERNEL_START_FAILED;
    }



    LOG(ANSI_GREEN "RCFiosOverlay init OK !\n" ANSI_RESET);
    FluxFOpen_hook_UID = taiHookFunctionOffset(&FluxFOpen_hookref, info.modid, 0, idx, 1, FluxFOpen_hookfunction);
    if ( FluxFOpen_hook_UID < 0 ) {
        LOG_ERROR("Hooking FluxFOpen returned 0x%08X < 0 !\nExiting...\n", FluxFOpen_hook_UID);
        return SCE_KERNEL_START_FAILED;
    }
    else { 
        LOG("Hooked FluxFOpen with success, enjoy :)\n");
        return SCE_KERNEL_START_SUCCESS;
    }
}

int module_stop(SceSize argc, const void* args){
    if (FluxFOpen_hook_UID >= 0) taiHookRelease(FluxFOpen_hook_UID, FluxFOpen_hookref);
    sceFiosOverlayRemove(overlayID);
    return SCE_KERNEL_STOP_SUCCESS;
}