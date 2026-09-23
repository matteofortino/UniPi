#include "../internal.h"

namespace svga {

	volatile natb* config(natw max_screenx, natw max_screeny)
	{
		if (framebuffer == nullptr)
			init();

		//innanzitutto disablitiamo le estensioni BOCHS VBE, come richiesto da specifiche
		vbeext[VBE_DISPI_INDEX_ENABLE] = VBE_DISPI_DISABLED;

		//settiamo risoluzione e profondità di colore
		vbeext[VBE_DISPI_INDEX_XRES] = max_screenx;
		vbeext[VBE_DISPI_INDEX_YRES] = max_screeny;
		vbeext[VBE_DISPI_INDEX_BPP] = VBE_DISPI_BPP_8;

		//chiediamo la modalità di buffer lineare e riattiviamo le estensioni
		vbeext[VBE_DISPI_INDEX_ENABLE] = VBE_DISPI_ENABLED | VBE_DISPI_LFB_ENABLED;

		return static_cast<volatile natb*>(framebuffer);
	}

}
