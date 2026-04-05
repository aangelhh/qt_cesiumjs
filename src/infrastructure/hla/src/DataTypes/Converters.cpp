#include "DataTypes/Converters.h"
#include <cmath>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iconv.h>

#include "../../include/utils/hla_utils.hpp"


namespace SDL_RTI
{

	Converters::Converters()
	{
	}

	Converters::~Converters()
	{
	}

	float Converters::convert_xyz_to_lat(DevStudio::SpatialVariantStruct val) 
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float a = 6378137.0;  // Semi-major axis, meters
		float e = static_cast<float>(8.1819190842622e-2);
		float b = 0.0f;
		float ep = 0.0f;
		float p = 0.0f;
		float th = 0.0f;
		float lon = 0.0f;
		float lat = 0.0f;
		float N = 0.0f;
		float alt = 0.0f;switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			x = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			x = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			x = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			x = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			x = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			x = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			x = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			x = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			x = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getZ());

			break;
		}

		b = std::sqrt(a * a * (1 - e * e));
		ep = std::sqrt((a * a - b * b) / (b * b));
		p = std::sqrt(x * x + y * y);
		th = std::atan2(a * z, b * p);

		lon = std::atan2(y, x);
		lat = (std::atan2((z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		N = (a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LATlat: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		return lat;
	}

	float Converters::convert_xyz_to_lon(DevStudio::SpatialVariantStruct val)
	{
		float x=0.0f;
		float y=0.0f;
		float z=0.0f;
		float a = 6378137.0;  // Semi-major axis, meters
		float e = static_cast<float>(8.1819190842622e-2);
		float b=0.0f;
		float ep=0.0f;
		float p=0.0f;
		float th=0.0f;
		float lon=0.0f;
		float lat=0.0f;
		float N=0.0f;
		float alt=0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			x = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			x = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			x = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			x = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			x = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			x = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			x = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			x = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			x = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getZ());

			break;
		}
		b = std::sqrt(a * a * (1 - e * e));
		ep = std::sqrt((a * a - b * b) / (b * b));
		p = std::sqrt(x * x + y * y);
		th = std::atan2(a * z, b * p);

		lon = std::atan2(y, x);
		lat = static_cast<float>(std::atan2((z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		N = static_cast<float>(a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LATlon: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		return lon;

	}

	float Converters::convert_xyz_to_alt(DevStudio::SpatialVariantStruct val)
	{
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float a = 6378137.0;  // Semi-major axis, meters
		float e = static_cast<float>(8.1819190842622e-2);
		float b = 0.0f;
		float ep = 0.0f;
		float p = 0.0f;
		float th = 0.0f;
		float lon = 0.0f;
		float lat = 0.0f;
		float N = 0.0f;
		float alt = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			x = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialStatic()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			x = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFPB()->getWorldLocation().getZ());
			

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			x = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFPW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			x = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFVB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			x = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialFVW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			x = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRPB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			x = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRPW()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			x = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRVB()->getWorldLocation().getZ());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			x = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getX());
			y = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getY());
			z = static_cast<float>(val.getSpatialRVW()->getWorldLocation().getZ());

			break;
		}

		//std::cout << "X:" << x << std::endl;
		//std::cout << "Y:" << y << std::endl;
		//std::cout << "Z:" << z << std::endl;

		b = std::sqrt(a * a * (1 - e * e));
		ep = std::sqrt((a * a - b * b) / (b * b));
		p = std::sqrt(x * x + y * y);
		th = std::atan2(a * z, b * p);

		lon = std::atan2(y, x);
		lat = static_cast<float>(std::atan2((z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		N = static_cast<float>(a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LATalt: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		return alt;
	}

	double Converters::meterToFeet(float altitudeMeter)
	{
		const float feetToMeter = static_cast<float>(3.28084);
		float altitudeFeet = altitudeMeter * feetToMeter;
		return altitudeFeet;
	}

	double Converters::calculate_heading(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll)
	{
		lat = lat * 3.14159265358979323846 / 180;
		lon = lon * 3.14159265358979323846 / 180;
		EulerToHeadingPitchRoll(lat, lon, psi, theta, phi, heading, pitch, roll);
		float grados = static_cast<float>((heading * 180 / 3.14159265358979323846));
		if (grados < 0) {
			grados = grados + 360;
		}
		if (grados == 360) {
			grados = 0;
		}
		

		return (grados);
		
	}
	double Converters::calculate_pitch(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll)
	{
		lat = lat * 3.14159265358979323846 / 180;
		lon = lon * 3.14159265358979323846 / 180;
		EulerToHeadingPitchRoll(lat, lon, psi, theta, phi, heading, pitch, roll);
		float grados = static_cast<float>((heading * 180 / 3.14159265358979323846));
		if (grados < 0) {
			grados = grados + 360;
		}
		if (grados == 360) {
			grados = 0;
		}

		return ((pitch * 180 / 3.14159265358979323846));

	}
	double Converters::calculate_roll(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll)
	{
		lat = lat * 3.14159265358979323846 / 180;
		lon = lon * 3.14159265358979323846 / 180;
		EulerToHeadingPitchRoll(lat, lon, psi, theta, phi, heading, pitch, roll);
		float grados = static_cast<float>((heading * 180 / 3.14159265358979323846));
		
		if (grados < 0) {
			grados = grados + 360;
		}
		if (grados == 360) {
			grados = 0;
		}

		return (roll * 180 / 3.14159265358979323846);

	}

	DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum Converters::calculate_spatial_discriminant(double& speedX, double& speedY, double& speedZ, double& accelX, double& accelY, double& accelZ, double& angularVelocityX, double& angularVelocityY, double& angularVelocityZ)
	{
		if (speedX == 0 && speedY == 0 && speedZ == 0 && accelX == 0 && accelY == 0 && accelZ == 0 && angularVelocityX == 0 && angularVelocityY == 0 && angularVelocityZ == 0)
			return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum::STATIC_;
		else if((speedX != 0 || speedY != 0 || speedZ != 0) && accelX == 0 && accelY == 0 && accelZ == 0 && angularVelocityX == 0 && angularVelocityY == 0 && angularVelocityZ == 0)
			return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum::DRM_FPW;
		else if ((speedX != 0 || speedY != 0 || speedZ != 0) && accelX == 0 && accelY == 0 && accelZ == 0 && (angularVelocityX != 0 || angularVelocityY != 0 || angularVelocityZ != 0))
			return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum::DRM_RPW;
		else if ((speedX != 0 || speedY != 0 || speedZ != 0) && (accelX != 0 || accelY != 0 || accelZ != 0) && angularVelocityX == 0 && angularVelocityY == 0 && angularVelocityZ == 0)
			return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum::DRM_FVW;
		else if ((speedX != 0 || speedY != 0 || speedZ != 0) && (accelX != 0 || accelY != 0 || accelZ != 0) && (angularVelocityX != 0 || angularVelocityY != 0 || angularVelocityZ != 0))
			return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum::DRM_RVW;
		else
			return DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum();
	}

	float Converters::calculate_velocity_X(DevStudio::SpatialVariantStruct val)
	{
		float velocityX=0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			velocityX = static_cast<float>(val.getSpatialFPB()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			velocityX = static_cast<float>(val.getSpatialFPW()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			velocityX = static_cast<float>(val.getSpatialFVB()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			velocityX = static_cast<float>(val.getSpatialFVW()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			velocityX = static_cast<float>(val.getSpatialRPB()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			velocityX = static_cast<float>(val.getSpatialRPW()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			velocityX = static_cast<float>(val.getSpatialRVB()->getVelocityVector().getXVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			velocityX = static_cast<float>(val.getSpatialRVW()->getVelocityVector().getXVelocity());

			break;
		}
		return velocityX;
	}
	float Converters::calculate_velocity_Y(DevStudio::SpatialVariantStruct val)
	{
		float velocityY = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			velocityY = static_cast<float>(val.getSpatialFPB()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			velocityY = static_cast<float>(val.getSpatialFPW()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			velocityY = static_cast<float>(val.getSpatialFVB()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			velocityY = static_cast<float>(val.getSpatialFVW()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			velocityY = static_cast<float>(val.getSpatialRPB()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			velocityY = static_cast<float>(val.getSpatialRPW()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			velocityY = static_cast<float>(val.getSpatialRVB()->getVelocityVector().getYVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			velocityY = static_cast<float>(val.getSpatialRVW()->getVelocityVector().getYVelocity());

			break;
		}
		return velocityY;
	}
	float Converters::calculate_velocity_Z(DevStudio::SpatialVariantStruct val)
	{
		float velocityZ = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			velocityZ = static_cast<float>(val.getSpatialFPB()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			velocityZ = static_cast<float>(val.getSpatialFPW()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			velocityZ = static_cast<float>(val.getSpatialFVB()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			velocityZ = static_cast<float>(val.getSpatialFVW()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			velocityZ = static_cast<float>(val.getSpatialRPB()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			velocityZ = static_cast<float>(val.getSpatialRPW()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			velocityZ = static_cast<float>(val.getSpatialRVB()->getVelocityVector().getZVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			velocityZ = static_cast<float>(val.getSpatialRVW()->getVelocityVector().getZVelocity());

			break;
		}
		return velocityZ;
	}

	float Converters::calculate_acceleration_X(DevStudio::SpatialVariantStruct val)
	{
		float accelerationX = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			accelerationX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			accelerationX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			accelerationX = static_cast<float>(val.getSpatialFVB()->getAccelerationVector().getXAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			accelerationX = static_cast<float>(val.getSpatialFVW()->getAccelerationVector().getXAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			accelerationX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			accelerationX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			accelerationX = static_cast<float>(val.getSpatialRVB()->getAccelerationVector().getXAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			accelerationX = static_cast<float>(val.getSpatialRVW()->getAccelerationVector().getXAcceleration());

			break;
	}
		return accelerationX;
	}
	float Converters::calculate_acceleration_Y(DevStudio::SpatialVariantStruct val)
	{
		float accelerationY = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			accelerationY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			accelerationY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			accelerationY = static_cast<float>(val.getSpatialFVB()->getAccelerationVector().getYAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			accelerationY = static_cast<float>(val.getSpatialFVW()->getAccelerationVector().getYAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			accelerationY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			accelerationY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			accelerationY = static_cast<float>(val.getSpatialRVB()->getAccelerationVector().getYAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			accelerationY = static_cast<float>(val.getSpatialRVW()->getAccelerationVector().getYAcceleration());

			break;
		}
		return accelerationY;
	}
	float Converters::calculate_acceleration_Z(DevStudio::SpatialVariantStruct val)
	{
		float accelerationZ = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			accelerationZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			accelerationZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			accelerationZ = static_cast<float>(val.getSpatialFVB()->getAccelerationVector().getZAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			accelerationZ = static_cast<float>(val.getSpatialFVW()->getAccelerationVector().getZAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			accelerationZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			accelerationZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			accelerationZ = static_cast<float>(val.getSpatialRVB()->getAccelerationVector().getZAcceleration());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			accelerationZ = static_cast<float>(val.getSpatialRVW()->getAccelerationVector().getZAcceleration());

			break;
		}
		return accelerationZ;
	}

	float Converters::calculate_angular_velocity_X(DevStudio::SpatialVariantStruct val)
	{
		float angularVelocityX = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			angularVelocityX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			angularVelocityX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			angularVelocityX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			angularVelocityX = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			angularVelocityX = static_cast<float>(val.getSpatialRPB()->getAngularVelocity().getXAngularVelocity());;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			angularVelocityX = static_cast<float>(val.getSpatialRPW()->getAngularVelocity().getXAngularVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			angularVelocityX = static_cast<float>(val.getSpatialRVB()->getAngularVelocity().getXAngularVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			angularVelocityX = static_cast<float>(val.getSpatialRVW()->getAngularVelocity().getXAngularVelocity());

			break;
		}
		return angularVelocityX;
	}
	float Converters::calculate_angular_velocity_Y(DevStudio::SpatialVariantStruct val)
	{
		float angularVelocityY = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			angularVelocityY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			angularVelocityY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			angularVelocityY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			angularVelocityY = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			angularVelocityY = static_cast<float>(val.getSpatialRPB()->getAngularVelocity().getXAngularVelocity());;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			angularVelocityY = static_cast<float>(val.getSpatialRPW()->getAngularVelocity().getXAngularVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			angularVelocityY = static_cast<float>(val.getSpatialRVB()->getAngularVelocity().getXAngularVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			angularVelocityY = static_cast<float>(val.getSpatialRVW()->getAngularVelocity().getXAngularVelocity());

			break;
		}
		return angularVelocityY;
	}
	float Converters::calculate_angular_velocity_Z(DevStudio::SpatialVariantStruct val)
	{
		float angularVelocityZ = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			angularVelocityZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			angularVelocityZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			angularVelocityZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			angularVelocityZ = 0;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			angularVelocityZ = static_cast<float>(val.getSpatialRPB()->getAngularVelocity().getXAngularVelocity());;

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			angularVelocityZ = static_cast<float>(val.getSpatialRPW()->getAngularVelocity().getXAngularVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			angularVelocityZ = static_cast<float>(val.getSpatialRVB()->getAngularVelocity().getXAngularVelocity());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			angularVelocityZ = static_cast<float>(val.getSpatialRVW()->getAngularVelocity().getXAngularVelocity());

			break;
		}
		return angularVelocityZ;
	}

	float Converters::calculate_velocity(float x, float y, float z)
	{
		return  sqrt(x * x + y * y + z * z);;
	}

	float Converters::calculate_Phi(DevStudio::SpatialVariantStruct val)
	{
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			return static_cast<float>(val.getSpatialStatic()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			return static_cast<float>(val.getSpatialFPB()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			return static_cast<float>(val.getSpatialFPW()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			return static_cast<float>(val.getSpatialFVB()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			return static_cast<float>(val.getSpatialFVW()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			return static_cast<float>(val.getSpatialRPB()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			return static_cast<float>(val.getSpatialRPW()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			return static_cast<float>(val.getSpatialRVB()->getOrientation().getPhi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			return static_cast<float>(val.getSpatialRVW()->getOrientation().getPhi());
		}
		return 0;
	}

	float Converters::calculate_Theta(DevStudio::SpatialVariantStruct val)
	{
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			return static_cast<float>(val.getSpatialStatic()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			return static_cast<float>(val.getSpatialFPB()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			return static_cast<float>(val.getSpatialFPW()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			return static_cast<float>(val.getSpatialFVB()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			return static_cast<float>(val.getSpatialFVW()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			return static_cast<float>(val.getSpatialRPB()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			return static_cast<float>(val.getSpatialRPW()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			return static_cast<float>(val.getSpatialRVB()->getOrientation().getTheta());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			return static_cast<float>(val.getSpatialRVW()->getOrientation().getTheta());
		}
		return 0;
	}

	float Converters::calculate_Psi(DevStudio::SpatialVariantStruct val)
	{
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			return static_cast<float>(val.getSpatialStatic()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			return static_cast<float>(val.getSpatialFPB()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			return static_cast<float>(val.getSpatialFPW()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			return static_cast<float>(val.getSpatialFVB()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			return static_cast<float>(val.getSpatialFVW()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			return static_cast<float>(val.getSpatialRPB()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			return static_cast<float>(val.getSpatialRPW()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			return static_cast<float>(val.getSpatialRVB()->getOrientation().getPsi());
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			return static_cast<float>(val.getSpatialRVW()->getOrientation().getPsi());
		}
		return 0;
	}

	float Converters::calculate_Roll(DevStudio::SpatialVariantStruct val)
	{

		float phi = 0.0f;
		float psi = 0.0f;
		float theta = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			phi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialStatic()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			phi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			phi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			phi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			phi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			
			phi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPB()->getOrientation().getTheta());
			

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			phi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			phi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			phi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVW()->getOrientation().getTheta());

			break;
		}
		float heading = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;

		return calculate_roll(convert_xyz_to_lat(val), convert_xyz_to_lon(val), psi, theta, phi, heading, pitch, roll);
	}

	float Converters::calculate_Pitch(DevStudio::SpatialVariantStruct val)
	{
		float phi = 0.0f;
		float psi = 0.0f;
		float theta = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			phi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialStatic()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			phi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			phi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			phi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			phi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:

			phi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPB()->getOrientation().getTheta());


			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			phi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			phi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			phi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVW()->getOrientation().getTheta());

			break;
		}
		float heading = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;

		return calculate_pitch(convert_xyz_to_lat(val),convert_xyz_to_lon(val),psi,theta,phi,heading,pitch,roll);
	}

	bool Converters::calculate_IsFrozen(DevStudio::SpatialVariantStruct val)
	{
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			return val.getSpatialStatic()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			return val.getSpatialFPB()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			return val.getSpatialFPW()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			return val.getSpatialFVB()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			return val.getSpatialFVW()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			return val.getSpatialRPB()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			return val.getSpatialRPW()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			return val.getSpatialRVB()->getIsFrozen();
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			return val.getSpatialRVW()->getIsFrozen();
		}

		return true;
	}

	float Converters::calculate_Heading(DevStudio::SpatialVariantStruct val)
	{
		float phi = 0.0f;
		float psi = 0.0f;
		float theta = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			phi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialStatic()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			phi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			phi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			phi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			phi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:

			phi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPB()->getOrientation().getTheta());


			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			phi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			phi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			phi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVW()->getOrientation().getTheta());

			break;
		}
		float heading = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;

		return calculate_heading(convert_xyz_to_lat(val), convert_xyz_to_lon(val), psi, theta, phi, heading, pitch, roll);
	}

	float Converters::calculate_Yaw(DevStudio::SpatialVariantStruct val)
	{
		float phi = 0.0f;
		float psi = 0.0f;
		float theta = 0.0f;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			phi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialStatic()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialStatic()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			phi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			phi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			phi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			phi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialFVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialFVW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:

			phi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPB()->getOrientation().getTheta());


			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			phi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRPW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRPW()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			phi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVB()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVB()->getOrientation().getTheta());

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			phi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPhi());
			psi = static_cast<float>(val.getSpatialRVW()->getOrientation().getPsi());
			theta = static_cast<float>(val.getSpatialRVW()->getOrientation().getTheta());

			break;
		}
		float heading = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;

		return theta * 180.0 / 3.14159265358979323;
	}

	DevStudio::WorldLocationStruct Converters::calculate_WorldLocation(DevStudio::SpatialVariantStruct val)
	{
		DevStudio::WorldLocationStruct worldLocation;
		switch (val.getDiscriminant()) {
		case DevStudio::DeadReckoningAlgorithmEnum::STATIC_:
			worldLocation = val.getSpatialStatic()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPB:
			worldLocation = val.getSpatialFPB()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FPW:
			worldLocation = val.getSpatialFPW()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVB:
			worldLocation = val.getSpatialFVB()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_FVW:
			worldLocation = val.getSpatialFVW()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPB:
			worldLocation = val.getSpatialRPB()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RPW:
			worldLocation = val.getSpatialRPW()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVB:
			worldLocation = val.getSpatialRVB()->getWorldLocation();

			break;
		case DevStudio::DeadReckoningAlgorithmEnum::DRM_RVW:
			worldLocation = val.getSpatialRVW()->getWorldLocation();

			break;
		}
		float heading = 0.0f;
		float pitch = 0.0f;
		float roll = 0.0f;

		return worldLocation;
	}

	float Converters::convert_xyz_to_lat_WorldLocation(DevStudio::WorldLocationStruct val) {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float a = 6378137.0;  // Semi-major axis, meters
		float e = static_cast<float>(8.1819190842622e-2);
		float b = 0.0f;
		float ep = 0.0f;
		float p = 0.0f;
		float th = 0.0f;
		float lon = 0.0f;
		float lat = 0.0f;
		float N = 0.0f;
		float alt = 0.0f;
		
			x = static_cast<float>(val.getX());
			y = static_cast<float>(val.getY());
			z = static_cast<float>(val.getZ());

			
		b = std::sqrt(a * a * (1 - e * e));
		ep = std::sqrt((a * a - b * b) / (b * b));
		p = std::sqrt(x * x + y * y);
		th = std::atan2(a * z, b * p);

		lon = std::atan2(y, x);
		lat = (std::atan2((z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		N = (a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LAT: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		return lat;
	}
	float Converters::convert_xyz_to_lon_WorldLocation(DevStudio::WorldLocationStruct val) {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float a = 6378137.0;  // Semi-major axis, meters
		float e = static_cast<float>(8.1819190842622e-2);
		float b = 0.0f;
		float ep = 0.0f;
		float p = 0.0f;
		float th = 0.0f;
		float lon = 0.0f;
		float lat = 0.0f;
		float N = 0.0f;
		float alt = 0.0f;

		x = static_cast<float>(val.getX());
		y = static_cast<float>(val.getY());
		z = static_cast<float>(val.getZ());


		b = std::sqrt(a * a * (1 - e * e));
		ep = std::sqrt((a * a - b * b) / (b * b));
		p = std::sqrt(x * x + y * y);
		th = std::atan2(a * z, b * p);

		lon = std::atan2(y, x);
		lat = (std::atan2((z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		N = (a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LAT: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		return lon;
	}
	float Converters::convert_xyz_to_alt_WorldLocation(DevStudio::WorldLocationStruct val) {
		float x = 0.0f;
		float y = 0.0f;
		float z = 0.0f;
		float a = 6378137.0;  // Semi-major axis, meters
		float e = static_cast<float>(8.1819190842622e-2);
		float b = 0.0f;
		float ep = 0.0f;
		float p = 0.0f;
		float th = 0.0f;
		float lon = 0.0f;
		float lat = 0.0f;
		float N = 0.0f;
		float alt = 0.0f;

		x = static_cast<float>(val.getX());
		y = static_cast<float>(val.getY());
		z = static_cast<float>(val.getZ());


		b = std::sqrt(a * a * (1 - e * e));
		ep = std::sqrt((a * a - b * b) / (b * b));
		p = std::sqrt(x * x + y * y);
		th = std::atan2(a * z, b * p);

		lon = std::atan2(y, x);
		lat = (std::atan2((z + ep * ep * b * std::pow(std::sin(th), 3)), (p - e * e * a * std::pow(std::cos(th), 3))));
		N = (a / std::sqrt(1 - e * e * std::pow(std::sin(lat), 2)));
		alt = p / std::cos(lat) - N;

		// Convert radians to degrees
		lon = static_cast<float>(lon * 180.0 / 3.14159265358979323);
		lat = static_cast<float>(lat * 180.0 / 3.14159265358979323);
		alt = static_cast<float>(meterToFeet(alt));
		//std::cout << "LAT: " << lat << "LON: " << lon << "ALT: " << alt << std::endl;
		return alt;
	}

	void Converters::convert_latlonalt_to_xyz(double latitude, double longitude, double altitude, double& x, double& y, double& z)
	{
		// WGS84 ellipsoid constants
		const double a = 6378137.0; // semi-major axis in meters
		const double f = 1.0 / 298.257223563; // flattening
		const double e2 = f * (2.0 - f); // eccentricity squared

		float pi = 3.14159265358979323;
		double latRad = latitude * pi / 180.0; // latitude in radians
		double lonRad = longitude * pi / 180.0; // longitude in radians

		// Calculate the prime vertical radius of curvature
		double N = a / sqrt(1.0 - e2 * pow(sin(latRad), 2));

		// Calculate ECEF coordinates
		x = (N + altitude) * cos(latRad) * cos(lonRad);
		y = (N + altitude) * cos(latRad) * sin(lonRad);
		z = (N * (1.0 - e2) + altitude) * sin(latRad);
	}

	void Converters::convert_xyz_to_latlonalt(double x, double y, double z, double& latitude, double& longitude, double& altitude)
	{
		// WGS84 ellipsoid constants
		const double a = 6378137.0; // semi-major axis
		const double f = 1.0 / 298.257223563;
		const double e2 = f * (2.0 - f); // eccentricity squared
		const double b = a * (1.0 - f);  // semi-minor axis
		float pi = 3.14159265358979323;

		// Calculate longitude
		longitude = atan2(y, x);

		// Iterative calculation of latitude and altitude
		double p = sqrt(x * x + y * y);
		double theta = atan2(z * a, p * b);

		double sinTheta = sin(theta);
		double cosTheta = cos(theta);

		latitude = atan2(z + e2 * b * sinTheta * sinTheta * sinTheta,
			p - e2 * a * cosTheta * cosTheta * cosTheta);

		double N = a / sqrt(1.0 - e2 * sin(latitude) * sin(latitude));
		altitude = p / cos(latitude) - N;

		// Convert radians to degrees
		const double rad2deg = 180.0 / pi;
		latitude *= rad2deg;
		longitude *= rad2deg;
	}

	std::chrono::system_clock::time_point Converters::Convert_ClockTime_to_TimePoint(const DevStudio::ClockTimeStruct& clockTime)
	{
		// Definir la epoch (1 de enero de 1970)
		std::chrono::system_clock::time_point epoch = std::chrono::system_clock::from_time_t(0);

		// Calcular el número total de horas desde la epoch
		std::chrono::hours totalHours(clockTime.hours);

		// Convertir timePastTheHour a segundos
		const double scaleFactor = 3600.0 / (1LL << 31);
		std::chrono::duration<double> secondsPastTheHour(clockTime.timePastTheHour * scaleFactor);

		// Sumar las horas y los segundos a la epoch
		std::chrono::system_clock::time_point timePoint = epoch + totalHours + std::chrono::duration_cast<std::chrono::seconds>(secondsPastTheHour);

		return timePoint;
	}

	DevStudio::ClockTimeStruct Converters::Convert_TimePoint_ClockTime(const std::chrono::system_clock::time_point& timePoint)
	{
		DevStudio::ClockTimeStruct clockTime;

		// Definir la epoch (1 de enero de 1970)
		std::chrono::system_clock::time_point epoch = std::chrono::system_clock::from_time_t(0);

		// Calcular la diferencia en horas desde la epoch
		std::chrono::hours hoursSinceEpoch = std::chrono::duration_cast<std::chrono::hours>(timePoint - epoch);
		clockTime.hours = hoursSinceEpoch.count();

		// Obtener el tiempo actual en struct tm
		std::time_t now_time = std::chrono::system_clock::to_time_t(timePoint);

		// Calcular los segundos y milisegundos transcurridos desde el inicio de la hora
		auto durationSinceEpoch = timePoint.time_since_epoch();
		auto durationSinceHourStart = durationSinceEpoch % std::chrono::hours(1);

		// Convertir la duración a microsegundos
		auto microsecondsSinceHourStart = std::chrono::duration_cast<std::chrono::microseconds>(durationSinceHourStart).count();

		// Convertir los microsegundos a la unidad especificada (3600 / (2^31))
		const double scaleFactor = 3600.0 / (1LL << 31);
		clockTime.timePastTheHour = static_cast<unsigned int>(microsecondsSinceHourStart / 1'000'000 / scaleFactor);

		return clockTime;
	}

	std::string Converters::convert_wstring_to_string(const std::wstring& wstr) 
	{
		// Convertir wstring a string usando iconv
		iconv_t cd = iconv_open("UTF-8", "WCHAR_T");
		if (cd == (iconv_t)-1) {
			perror("iconv_open");
			return "";
		}

		size_t inbytesleft = wstr.size() * sizeof(wchar_t);
		size_t outbytesleft = wstr.size() * 4; // Estimación conservadora
		char* inbuf = (char*)wstr.data();
		char* outbuf = new char[outbytesleft];
		char* outptr = outbuf;

		size_t result = iconv(cd, &inbuf, &inbytesleft, &outptr, &outbytesleft);
		iconv_close(cd);

		if (result == (size_t)-1) {
			perror("iconv");
			delete[] outbuf;
			return "";
		}

		std::string str(outbuf, outptr - outbuf);
		delete[] outbuf;
		return str;
	}

	std::wstring Converters::convert_string_to_wstring(const std::string& str) 
	{
		// Convertir string a wstring usando iconv
		iconv_t cd = iconv_open("WCHAR_T", "UTF-8");
		if (cd == (iconv_t)-1) {
			perror("iconv_open");
			return L"";
		}

		size_t inbytesleft = str.size();
		size_t outbytesleft = str.size() * sizeof(wchar_t); // Estimación conservadora
		char* inbuf = const_cast<char*>(str.data());
		wchar_t* outbuf = new wchar_t[outbytesleft];
		wchar_t* outptr = outbuf;

		size_t result = iconv(cd, &inbuf, &inbytesleft, reinterpret_cast<char**>(&outptr), &outbytesleft);
		iconv_close(cd);

		if (result == (size_t)-1) {
			perror("iconv");
			delete[] outbuf;
			return L"";
		}

		std::wstring wstr(outbuf, outptr - outbuf);
		delete[] outbuf;
		return wstr;
	}

	std::string Converters::convert_marking_to_string(const DevStudio::MarkingStruct& marking)
	{
		size_t end = marking.markingData.size();
		while (end > 0 && marking.markingData[end - 1] == '\0') {
			--end;
		}
		std::string str(marking.markingData.begin(), marking.markingData.begin() + end);
		return str;
	}
	DevStudio::MarkingStruct Converters::convert_string_to_marking(const std::string& sMarking)
	{
		DevStudio::MarkingStruct marking{ DevStudio::MarkingEncodingEnum::MarkingEncodingEnum::ASCII, std::vector<char>(11, '\0') };
		for (int i = 0; i < sMarking.length() && i < 11; i++)
			marking.markingData[i] = sMarking[i];
		return marking;
	}
}