// $Id$

#ifndef TURBORFDC_HH
#define TURBORFDC_HH

#include "MSXFDC.hh"
#include <memory>

namespace openmsx {

class TC8566AF;

class TurboRFDC : public MSXFDC
{
public:
	TurboRFDC(MSXMotherBoard& motherBoard, const XMLElement& config,
	          const EmuTime& time);
	virtual ~TurboRFDC();

	virtual void reset(const EmuTime& time);

	virtual byte readMem(word address, const EmuTime& time);
	virtual byte peekMem(word address, const EmuTime& time) const;
	virtual void writeMem(word address, byte value, const EmuTime& time);
	virtual const byte* getReadCacheLine(word start) const;
	virtual byte* getWriteCacheLine(word address) const;

private:
	void setBank(byte value);

	const std::auto_ptr<TC8566AF> controller;
	const byte* memory;
	byte blockMask;
};

} // namespace openmsx

#endif
