﻿#pragma once // src\Simulation\IVehicle.h - vehicle iface
namespace syscross::TraffModel::Sim {
class Road;
struct IVehicle {
	virtual ~IVehicle() {}
    virtual void update(IVehicle* lead, double dt, Road* road) = 0;
    virtual void stop(double t) = 0;
    virtual void unstop(double t) = 0;
    virtual double x() const = 0;
    virtual double v() const = 0;
    virtual uint length() const = 0;
    virtual uint width() const = 0;
	virtual std::vector< uint > const& path() const = 0;
    virtual uint currentIdxRoadIndex() const = 0;
	virtual void resetPositionOnRoad() = 0;
	virtual void incCurrentIdxRoadIndex() = 0;
    virtual void setVehicleIndex(uint index) = 0;
	virtual uint getVehicleIndex() const = 0;
	virtual void setPositionOnRoad(qreal x) = 0;
	virtual void setIdxRoadIndex(uint idxRoadIndex) = 0;

    virtual void slow(double traffic_light_slow_factor) = 0;
    virtual void unslow() = 0;
	typedef std::deque< IVehicle * > vehicles_t;
};
} // namespace syscross::TraffModel::Sim
