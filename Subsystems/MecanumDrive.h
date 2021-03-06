#pragma once

#include <WPILib.h>
#include "../Robotmap.h"
#include "../Commands/DriveWithJoystick.h"
#include "SubsystemWithCommand.hpp"

class MecanumDrive: public SubsystemWithCommand<DriveWithJoystick>, private RobotDrive {
private:
	CANJaguar m_frontLeft;
	CANJaguar m_frontRight;
	CANJaguar m_backLeft;
	CANJaguar m_backRight;
public:
	enum DriveMode {
		PERCENT_VBUS,
		VOLTAGE,
		CURRENT,
		SPEED,
		POSITION,
	};
private:
	bool flipped;
	bool majorAxisOnly;
	
	DriveMode m_driveMode;
public:
	MecanumDrive();
	
	void Drive(double x, double y, double w);
	void SetDriveMode(DriveMode mode);
	void SetSafetyEnabled(bool enabled);
	void Flip();
	void SetMajorAxisMode(bool on);
	void Init();
};
