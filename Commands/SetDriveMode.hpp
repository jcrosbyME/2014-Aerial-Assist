#pragma once

#include "Commands/Command.h"
#include "../Main.h"

enum DriveMode {
	MAJOR_AXIS_ONLY,
	BOTH_AXES,
	ENCODERS_ON,
	ENCODERS_OFF,
	SWAP_SIDES,
};

template<DriveMode mode>
class SetDriveMode: public Command {
public:
	SetDriveMode() {}
	virtual void Initialize() {
		printf("Setting drive mode");
		switch(mode) {
		case MAJOR_AXIS_ONLY:
			printf(" Major axis\n");
			Main::getDrive().SetMajorAxisMode(true);
			break;
		case BOTH_AXES:
			printf(" Both axes\n");
			Main::getDrive().SetMajorAxisMode(false);
			break;
		case ENCODERS_ON:
			printf(" Encoders on\n");
			//Main::getDrive().EnableEncoders();
			Main::getDrive().SetDriveMode(MecanumDrive::SPEED);
			break;
		case ENCODERS_OFF:
			printf(" Encoders off\n");
			//Main::getDrive().DisableEncoders();
			Main::getDrive().SetDriveMode(MecanumDrive::PERCENT_VBUS);
			break;
		case SWAP_SIDES:
			printf(" Swap sides\n");
			Main::getDrive().Flip();
		}
	}
	
	virtual void Execute() {}
	virtual bool IsFinished() { return true; }
	virtual void End() {}
	virtual void Interrupted() {}
};
