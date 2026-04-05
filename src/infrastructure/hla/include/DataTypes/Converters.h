#pragma once
#include <cmath>
#include <chrono>
#include <ctime>
#include <DevStudio/datatypes/SpatialVariantStruct.h>
#include <DevStudio/datatypes/SpatialVariantStruct.h>
#include <DevStudio/datatypes/EntityIdentifierStruct.h>
#include <DevStudio/datatypes/ClockTimeStruct.h>
#include <DevStudio/datatypes/MarkingStruct.h>

namespace SDL_RTI {

	struct CartesianCoordinates {
		double x;
		double y;
		double z;

		CartesianCoordinates() : x(0.0), y(0.0), z(0.0) {}

		CartesianCoordinates(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

		CartesianCoordinates(const CartesianCoordinates& other)
			: x(other.x), y(other.y), z(other.z) {}

		CartesianCoordinates& operator=(const CartesianCoordinates& other) {
			if (this != &other) {
				x = other.x;
				y = other.y;
				z = other.z;
			}
			return *this;
		}
	};
	class Converters {
		public:
			Converters();
			~Converters();
			static float convert_xyz_to_lat(DevStudio::SpatialVariantStruct val);
			static float convert_xyz_to_lon(DevStudio::SpatialVariantStruct val);
			static float convert_xyz_to_alt(DevStudio::SpatialVariantStruct val);
			static double meterToFeet(float altitudeMeter);
			static double calculate_heading(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll);
			static double calculate_pitch(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll);
			static double calculate_roll(double lat, double lon, double psi, double theta, double phi, double heading, double pitch, double roll);
			static DevStudio::DeadReckoningAlgorithmEnum::DeadReckoningAlgorithmEnum calculate_spatial_discriminant(double& speedX, double& speedY, double& speedZ, double& accelX, double& accelY, double& accelZ, double& angularVelocityX, double& angularVelocityY, double& angularVelocityZ);
			static bool calculate_IsFrozen(DevStudio::SpatialVariantStruct val);
			static float calculate_velocity_X(DevStudio::SpatialVariantStruct val);
			static float calculate_velocity_Y(DevStudio::SpatialVariantStruct val);
			static float calculate_velocity_Z(DevStudio::SpatialVariantStruct val);
			static float calculate_acceleration_X(DevStudio::SpatialVariantStruct val);
			static float calculate_acceleration_Y(DevStudio::SpatialVariantStruct val);
			static float calculate_acceleration_Z(DevStudio::SpatialVariantStruct val);
			static float calculate_angular_velocity_X(DevStudio::SpatialVariantStruct val);
			static float calculate_angular_velocity_Y(DevStudio::SpatialVariantStruct val);
			static float calculate_angular_velocity_Z(DevStudio::SpatialVariantStruct val);
			static float calculate_velocity(float x, float y, float z);
			static float calculate_Phi(DevStudio::SpatialVariantStruct val);
			static float calculate_Theta(DevStudio::SpatialVariantStruct val);
			static float calculate_Psi(DevStudio::SpatialVariantStruct val);
			static float calculate_Roll(DevStudio::SpatialVariantStruct val);
			static float calculate_Pitch(DevStudio::SpatialVariantStruct val);
			static float calculate_Heading(DevStudio::SpatialVariantStruct val);
			static float calculate_Yaw(DevStudio::SpatialVariantStruct val);
			static DevStudio::WorldLocationStruct calculate_WorldLocation(DevStudio::SpatialVariantStruct val);
			static float convert_xyz_to_lat_WorldLocation(DevStudio::WorldLocationStruct val);
			static float convert_xyz_to_lon_WorldLocation(DevStudio::WorldLocationStruct val);
			static float convert_xyz_to_alt_WorldLocation(DevStudio::WorldLocationStruct val);

			static void convert_latlonalt_to_xyz(double latitude, double longitude, double altitude, double& x, double& y, double& z);
			static void convert_xyz_to_latlonalt(double x, double y, double z, double& latitude, double& longitude, double& altitude);

			static std::chrono::system_clock::time_point Convert_ClockTime_to_TimePoint(const DevStudio::ClockTimeStruct& clockTime);
			static DevStudio::ClockTimeStruct Convert_TimePoint_ClockTime(const std::chrono::system_clock::time_point& timePoint);

			static std::string convert_wstring_to_string(const std::wstring& wstr);
			static std::wstring convert_string_to_wstring(const std::string& str);

			static std::string convert_marking_to_string(const DevStudio::MarkingStruct & marking);
			static DevStudio::MarkingStruct convert_string_to_marking(const std::string& marking);
	};

}

