// $Id$

#include "PrinterPortLogger.hh"
#include "PluggingController.hh"


PrinterPortLogger::PrinterPortLogger()
{
	file = NULL;
	prevStrobe = true;
	PluggingController::instance()->registerPluggable(this);
}

PrinterPortLogger::~PrinterPortLogger()
{
	PluggingController::instance()->unregisterPluggable(this);
	delete file;
}

bool PrinterPortLogger::getStatus(const EmuTime &time)
{
	return false;	// false = low = ready
}

void PrinterPortLogger::setStrobe(bool strobe, const EmuTime &time)
{
	if (strobe) {
		PRT_DEBUG("PRINTER: strobe high");
	} else {
		PRT_DEBUG("PRINTER: strobe low");
	}
	if (!strobe && prevStrobe) {
		// falling edge
		PRT_DEBUG("PRINTER: save in printlog file "<<toPrint);
		file->write(&toPrint, 1);
	}
	prevStrobe = strobe;
}

void PrinterPortLogger::writeData(byte data, const EmuTime &time)
{
	PRT_DEBUG("PRINTER: setting data "<<data);
	toPrint = data;
}

void PrinterPortLogger::plug(const EmuTime &time)
{
	std::string filename("printer.log");	// TODO read from config
	file = FileOpener::openFileTruncate(filename);
}

void PrinterPortLogger::unplug(const EmuTime &time)
{
	delete file;
	file = NULL;
}

const std::string &PrinterPortLogger::getName()
{
	return name;
}
const std::string PrinterPortLogger::name("logger");
