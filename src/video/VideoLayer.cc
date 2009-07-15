// $Id$

#include "VideoLayer.hh"
#include "RenderSettings.hh"
#include "Display.hh"
#include "CommandController.hh"
#include "GlobalSettings.hh"
#include "BooleanSetting.hh"
#include "VideoSourceSetting.hh"
#include "MSXEventDistributor.hh"
#include "MSXMotherBoard.hh"
#include "Event.hh"
#include "openmsx.hh"
#include <cassert>

namespace openmsx {

VideoLayer::VideoLayer(MSXMotherBoard& motherBoard_,
                       VideoSource videoSource_,
                       Display& display_)
	: motherBoard(motherBoard_)
	, display(display_)
	, renderSettings(display.getRenderSettings())
	, videoSourceSetting(renderSettings.getVideoSource())
	, videoSourceActivator(new VideoSourceActivator(
              videoSourceSetting, videoSource_))
	, powerSetting(motherBoard.getCommandController().
	                   getGlobalSettings().getPowerSetting())
	, videoSource(videoSource_)
	, transparency(false)
{
	calcCoverage();
	calcZ();
	display.addLayer(*this);

	videoSourceSetting.attach(*this);
	powerSetting.attach(*this);
	motherBoard.getMSXEventDistributor().registerEventListener(*this);
}

VideoLayer::~VideoLayer()
{
	PRT_DEBUG("Destructing VideoLayer...");
	motherBoard.getMSXEventDistributor().unregisterEventListener(*this);
	powerSetting.detach(*this);
	videoSourceSetting.detach(*this);

	display.removeLayer(*this);
	PRT_DEBUG("Destructing VideoLayer... DONE!");
}

VideoSource VideoLayer::getVideoSource() const
{
	return videoSource;
}

void VideoLayer::update(const Setting& setting)
{
	if (&setting == &videoSourceSetting) {
		calcZ();
	} else if (&setting == &powerSetting) {
		calcCoverage();
	}
}

void VideoLayer::calcZ()
{
	setZ((renderSettings.getVideoSource().getValue() == videoSource)
		? Z_MSX_ACTIVE
		: Z_MSX_PASSIVE);
}

void VideoLayer::calcCoverage()
{
	Coverage coverage;

	if (!powerSetting.getValue() || !motherBoard.isActive()) {
		coverage = COVER_NONE;
	} else if (transparency) {
		coverage = COVER_PARTIAL;
	} else {
		coverage = COVER_FULL;
	}

	setCoverage(coverage);
}

void VideoLayer::setTransparency(bool enabled)
{
	transparency = enabled;
	calcCoverage();
}

void VideoLayer::signalEvent(shared_ptr<const Event> event, EmuTime::param /*time*/)
{
	if ((event->getType() == OPENMSX_MACHINE_ACTIVATED) ||
	    (event->getType() == OPENMSX_MACHINE_DEACTIVATED)) {
		calcCoverage();
	}
}

} // namespace openmsx
