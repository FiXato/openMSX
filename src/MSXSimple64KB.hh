// $Id$

#ifndef __MSXSIMPLE64KB_HH__
#define __MSXSIMPLE64KB_HH__

#include "MSXMemDevice.hh"

// forward declarations
class EmuTime;


class MSXSimple64KB : public MSXMemDevice
{
	public:
		/**
		 * Constructor
		 */
		MSXSimple64KB(MSXConfig::Device *config, const EmuTime &time);

		/**
		 * Destructor
		 */
		~MSXSimple64KB();
		
		void reset(const EmuTime &time);
		
		byte readMem(word address, const EmuTime &time);
		void writeMem(word address, byte value, const EmuTime &time);  
		byte* getReadCacheLine(word start);
		byte* getWriteCacheLine(word start);

	private:
		byte* memoryBank;
		bool slowDrainOnReset;
};
#endif
