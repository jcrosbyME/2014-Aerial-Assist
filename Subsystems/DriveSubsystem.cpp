#include "DriveSubsystem.h"
#include <cmath>
#include "../Commands/DriveWithJoystick.h"
#include "../Robotmap.h"

DriveSubsystem::DriveSubsystem() try : 
	SubsystemWithCommand<DriveWithJoystick>("DriveSubsystem"), 
	P(0.50),
	I(0.01),
	D(0.00),
	n(0),
	m_majorAxisMode(false),
	frontLeft_alive(true),
	frontRight_alive(true),
	backLeft_alive(true),
	backRight_alive(true),
	backwards(false),
    m_frontLeft(kFrontLeftJaguar), 
    m_frontRight(kFrontRightJaguar), 
    m_backLeft(kBackLeftJaguar), 
    m_backRight(kBackRightJaguar),
    m_drive(m_frontLeft, m_backLeft, m_frontRight, m_backRight)
{} catch (...) {
	std::cout << "Exception caught in DriveSubsystem::DriveSubsystem\n"; //<< e.what() << std::endl;
}

void DriveSubsystem::init() 
{
	//m_drive.SetSafetyEnabled(false);
	// Invert the left motors.
	m_drive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
	m_drive.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
	m_drive.SetInvertedMotor(RobotDrive::kRearLeftMotor,  false);
	m_drive.SetInvertedMotor(RobotDrive::kRearRightMotor,  true);
	    
	// Make sure the timeout is reasonable
	m_drive.SetExpiration(0.25);

	// By default enable encoders. Either this function
	// or DisableEncoders() should be called, otherwise
	// the motors are not properly inverted or set up.
	//EnableEncoders();
	DisableEncoders();
}

bool DriveSubsystem::EnableEncoders(CANJaguar::ControlMode mode, float maxOutput/*bool invertGains*/) 
{
	switch(mode) {
	case CANJaguar::kSpeed:
		//m_drive.SetMaxOutput(kMaxDriveRPM);
		break;
	case CANJaguar::kPosition:
		//m_drive.SetMaxOutput(1.0);
		break;
	default:
		return false;
	}
	
	m_drive.SetMaxOutput(maxOutput);
	
	// When encoders are enabled, all motors must be inverted from
	// what they normaly would be.
	m_drive.SetInvertedMotor(RobotDrive::kFrontLeftMotor, false);
	m_drive.SetInvertedMotor(RobotDrive::kFrontRightMotor, true);
	m_drive.SetInvertedMotor(RobotDrive::kRearLeftMotor,  false);
	m_drive.SetInvertedMotor(RobotDrive::kRearRightMotor,  true);
	// Set max output to be max drive rpm, since the jaguars read
	// the encoder speed in rpm.
	
	// Disable encoders, if alread enabled.
	// Not sure this is necessary.
	m_frontLeft.DisableControl();
	m_backLeft.DisableControl();
	m_frontRight.DisableControl();
	m_backRight.DisableControl();
	
	// Max voltage should be 12V (Robot has 12V battery).
	m_frontLeft.ConfigMaxOutputVoltage(kMaxDriveVoltage);
	m_backLeft.ConfigMaxOutputVoltage(kMaxDriveVoltage);
	m_frontRight.ConfigMaxOutputVoltage(kMaxDriveVoltage);
	m_backRight.ConfigMaxOutputVoltage(kMaxDriveVoltage);
	
	// To use the encoders, we need to be in either
	// speed mode or position mode. For driving the 
	// robot, speed mode makes more sense.
	m_frontLeft.ChangeControlMode(mode);
	m_backLeft.ChangeControlMode(mode);
	m_frontRight.ChangeControlMode(mode);
	m_backRight.ChangeControlMode(mode);
	
	// Fetch PID gains from the smart dashboard.
	//P = SmartDashboard::GetNumber("Drive P");
	//I = SmartDashboard::GetNumber("Drive I");
	//D = SmartDashboard::GetNumber("Drive D");
	switch(mode) {
	case CANJaguar::kSpeed:
		P = 0.5;
		I = 0.01;
		D = 0.0;
		break;
	case CANJaguar::kPosition:
		P = SmartDashboard::GetNumber("Drive P");
		I = SmartDashboard::GetNumber("Drive I");
		D = SmartDashboard::GetNumber("Drive D");
		//P = 20.0;//SmartDashboard::GetNumber("Position P");
		//I = 0.0;//SmartDashboard::GetNumber("Position I");
		//D = 0.0;//SmartDashboard::GetNumber("Position D");
		break;
	default:
		break;
	}
	
	// Set proportional, integral and derivative gains,
	// inverting if appropriate.

	//if(invertGains) {
	m_frontLeft.SetPID(-P, -I, -D);
	m_backLeft.SetPID(-P, -I, -D);
	m_frontRight.SetPID(-P, -I, -D);
	m_backRight.SetPID(-P, -I, -D);
	/*} else {
		m_frontLeft.SetPID(P, I, D);
		m_backLeft.SetPID(P, I, D);
		m_frontRight.SetPID(P, I, D);
		m_backRight.SetPID(P, I, D);
	}*/
	
	// We have quadrature encoders
	switch(mode) {
	case CANJaguar::kSpeed:
		m_frontLeft.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
		m_backLeft.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
		m_frontRight.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
		m_backRight.SetSpeedReference(CANJaguar::kSpeedRef_QuadEncoder);
		//m_drive.SetMaxOutput(kMaxDriveRPM);
		break;
	case CANJaguar::kPosition:
		m_frontLeft.SetPositionReference(CANJaguar::kPosRef_QuadEncoder);
		m_backLeft.SetPositionReference(CANJaguar::kPosRef_QuadEncoder);
		m_frontRight.SetPositionReference(CANJaguar::kPosRef_QuadEncoder);
		m_backRight.SetPositionReference(CANJaguar::kPosRef_QuadEncoder);
		//m_drive.SetMaxOutput(1.0);
		break;
	default:
		return false;
	}
	
	// Set encoder pulses per rev
	m_frontLeft.ConfigEncoderCodesPerRev(kEncoderPulsesPerRev);
	m_backLeft.ConfigEncoderCodesPerRev(kEncoderPulsesPerRev);
	m_frontRight.ConfigEncoderCodesPerRev(kEncoderPulsesPerRev);
	m_backRight.ConfigEncoderCodesPerRev(kEncoderPulsesPerRev);
	// Enable encoder control with the parameters above.
	m_frontLeft.EnableControl();
	m_backLeft.EnableControl();
	m_frontRight.EnableControl();
	m_backRight.EnableControl();
	
	return true;
}	

void DriveSubsystem::DisableEncoders() 
{
	// Motors have to be inverted for using encoders, so they have
	// to be set back for kPercentVbus mode.
	m_drive.SetInvertedMotor(RobotDrive::kFrontLeftMotor,   true);
	m_drive.SetInvertedMotor(RobotDrive::kFrontRightMotor, false);
	m_drive.SetInvertedMotor(RobotDrive::kRearLeftMotor,    true);
	m_drive.SetInvertedMotor(RobotDrive::kRearRightMotor,  false);
		    
	// Output values for kPercentVbus mode are -1..1, so
	// max output should be 1.0
	m_drive.SetMaxOutput(1.0);
	// Disable encoder control.
	m_frontLeft.DisableControl();
	m_backLeft.DisableControl();
	m_frontRight.DisableControl();
	m_backRight.DisableControl();
	// Change mode to kPercentVbus.
	m_frontLeft.ChangeControlMode(CANJaguar::kPercentVbus);
	m_backLeft.ChangeControlMode(CANJaguar::kPercentVbus);
	m_frontRight.ChangeControlMode(CANJaguar::kPercentVbus);
	m_backRight.ChangeControlMode(CANJaguar::kPercentVbus);
}

void DriveSubsystem::EnableSafety() {
	m_drive.SetSafetyEnabled(true);
}

void DriveSubsystem::DisableSafety() {
	m_drive.SetSafetyEnabled(false);
}

void DriveSubsystem::Flip() {
	backwards = !backwards;
}

void DriveSubsystem::SetMajorAxisMode(bool on) {
	m_majorAxisMode = on;
}
    
void DriveSubsystem::DriveMecanum(float xVel, float yVel, float rotVel) 
{
	if(m_majorAxisMode) {
		if(fabs(xVel) > fabs(yVel)) yVel = 0.0f;
		else xVel = 0.0f;
	}
	
	if(backwards) {
		xVel = -xVel;
		yVel = -yVel;
	}
	
	// Drive Mecanum
	m_drive.MecanumDrive_Cartesian(xVel, yVel, rotVel);
	//m_drive.MecanumDrive_Cartesian(0,0,0);
	
	
	// Update smart dashboard with command, output voltage, 
	// registered output, and input voltage, for each Jaguar.
	// Only do this 5 times a second. 
	
	n++;
	if(n%5 == 0) 
	{
		if(m_frontLeft.IsAlive() || m_frontLeft.GetPowerCycled()) {
			frontLeft_alive = 10;
		}
		
		if(m_frontRight.IsAlive() || m_frontRight.GetPowerCycled()) {
			frontRight_alive = 10;
		}
		
		if(m_backLeft.IsAlive() || m_backLeft.GetPowerCycled()) {
			backLeft_alive = 10;
		}
		
		if(m_backRight.IsAlive() || m_backRight.GetPowerCycled()) {
			backRight_alive = 10;
		}
		
		if(frontLeft_alive > 0) --frontLeft_alive;
		if(frontRight_alive > 0) --frontRight_alive;
		if(backLeft_alive > 0) --backLeft_alive;
		if(backRight_alive > 0) --backRight_alive;
		
		//SmartDashboard::PutNumber("M4 Command", m_frontLeft.Get());
		SmartDashboard::PutNumber("M4 Output",  m_frontLeft.GetSpeed());
		//SmartDashboard::PutNumber("M4 Voltage", m_frontLeft.GetOutputVoltage());
		//SmartDashboard::PutNumber("M4 VBus", m_frontLeft.GetBusVoltage());
		//SmartDashboard::PutBoolean("M4 Alive",  frontLeft_alive == 0);
	
		//SmartDashboard::PutNumber("M2 Command", m_frontRight.Get());
		SmartDashboard::PutNumber("M2 Output",  m_frontRight.GetSpeed());
		//SmartDashboard::PutNumber("M2 Voltage", m_frontRight.GetOutputVoltage());
		//SmartDashboard::PutNumber("M2 VBus", m_frontRight.GetBusVoltage());
		//SmartDashboard::PutBoolean("M2 Alive",  frontRight_alive == 0);
		
		//SmartDashboard::PutNumber("M3 Command", m_backLeft.Get());
		SmartDashboard::PutNumber("M3 Output",  m_backLeft.GetSpeed());
		//SmartDashboard::PutNumber("M3 Voltage", m_backLeft.GetOutputVoltage());
		//SmartDashboard::PutNumber("M3 VBus", m_backLeft.GetBusVoltage());	
		//SmartDashboard::PutBoolean("M3 Alive",  backLeft_alive == 0);
		
		//SmartDashboard::PutNumber("M1 Command", m_backRight.Get());
		SmartDashboard::PutNumber("M1 Output",  m_backRight.GetSpeed());
		//SmartDashboard::PutNumber("M1 Voltage", m_backRight.GetOutputVoltage());
		//SmartDashboard::PutNumber("M1 VBus", m_backRight.GetBusVoltage());
		//SmartDashboard::PutBoolean("M1 Alive",  backRight_alive == 0);
		
		/*
		SmartDashboard::PutNumber("M1 Pos", m_backRight.GetPosition());
		SmartDashboard::PutNumber("M2 Pos", m_frontRight.GetPosition());
		SmartDashboard::PutNumber("M3 Pos", m_backLeft.GetPosition());
		SmartDashboard::PutNumber("M4 Pos", m_frontLeft.GetPosition());
		*/
	}
}

void DriveSubsystem::SetMaxOutput(float maxOutput) {
	m_drive.SetMaxOutput(maxOutput);
}
