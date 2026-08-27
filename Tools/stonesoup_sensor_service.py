#!/usr/bin/env python3
"""JSON-lines adapter exposing Stone Soup AESARadar probability calculations."""

from __future__ import annotations

import argparse
from datetime import datetime, timedelta, timezone
import json
import math
import sys

import numpy as np
import stonesoup
from stonesoup.models.measurement.nonlinear import CartesianToElevationBearingRange
from stonesoup.sensor.radar.beam_pattern import StationaryBeam
from stonesoup.sensor.radar.beam_shape import Beam2DGaussian
from stonesoup.sensor.radar.radar import AESARadar
from stonesoup.types.array import CovarianceMatrix, StateVector
from stonesoup.types.groundtruth import GroundTruthState


EPOCH = datetime(2000, 1, 1, tzinfo=timezone.utc)
RADAR_CACHE: dict[tuple[float, ...], AESARadar] = {}


def option(options: dict, name: str, default: float) -> float:
    return float(options.get(name, default))


def radar_for(options: dict) -> AESARadar:
    parameters = (
        option(options, "peakPowerWatts", 25000.0),
        option(options, "dutyCycle", 0.1),
        option(options, "bandwidthHz", 1.0e6),
        option(options, "receiverNoiseDb", 3.0),
        option(options, "frequencyHz", 10.0e9),
        option(options, "antennaGainDb", 35.0),
        option(options, "beamWidthDegrees", 120.0),
        option(options, "numberPulses", 16.0),
        option(options, "lossDb", 6.0),
        option(options, "probabilityFalseAlarm", 1.0e-6),
    )
    cached = RADAR_CACHE.get(parameters)
    if cached is not None:
        return cached

    measurement = CartesianToElevationBearingRange(
        ndim_state=3,
        mapping=(0, 1, 2),
        noise_covar=CovarianceMatrix(np.diag([1.0e-6, 1.0e-6, 25.0])),
    )
    radar = AESARadar(
        measurement_model=measurement,
        beam_shape=Beam2DGaussian(peak_power=parameters[0]),
        beam_transition_model=StationaryBeam(centre=(0.0, 0.0)),
        duty_cycle=parameters[1],
        band_width=parameters[2],
        receiver_noise=parameters[3],
        frequency=parameters[4],
        antenna_gain=parameters[5],
        beam_width=math.radians(parameters[6]),
        number_pulses=max(1, int(parameters[7])),
        loss=parameters[8],
        probability_false_alarm=parameters[9],
        rcs=1.0,
        swerling_on=False,
        position=StateVector([0.0, 0.0, 0.0]),
    )
    RADAR_CACHE[parameters] = radar
    return radar


def evaluate(request: dict) -> dict:
    options = request.get("options") or {}
    range_meters = max(1.0, float(request.get("rangeMeters", 0.0)))
    signature = max(0.01, float(request.get("targetSignature", 1.0)))
    rcs_scale = max(0.01, option(options, "rcsScaleSquareMeters", 1.0))
    configured_probability = min(
        1.0, max(0.0, float(request.get("configuredProbability", 1.0)))
    )
    timestamp = EPOCH + timedelta(
        seconds=max(0.0, float(request.get("simulationTimeSeconds", 0.0)))
    )
    truth = GroundTruthState(
        [[range_meters], [0.0], [0.0]], timestamp=timestamp
    )
    truth.rcs = signature * rcs_scale
    probability, snr, *_ = radar_for(options).gen_probability(truth)
    probability = min(1.0, max(0.0, float(probability) * configured_probability))
    return {
        "ok": True,
        "provider": "stone-soup",
        "providerVersion": stonesoup.__version__,
        "probability": probability,
        "snr": float(snr),
    }


def process(request: dict) -> dict:
    operation = request.get("operation")
    if operation == "health":
        return {
            "ok": True,
            "provider": "stone-soup",
            "providerVersion": stonesoup.__version__,
        }
    if operation == "evaluate":
        return evaluate(request)
    return {"ok": False, "error": f"Unsupported operation: {operation}"}


def run_stdio() -> int:
    for line in sys.stdin:
        try:
            request = json.loads(line)
            response = process(request)
        except Exception as exc:  # Keep protocol errors isolated per request.
            response = {"ok": False, "error": str(exc)}
        sys.stdout.write(json.dumps(response, separators=(",", ":")) + "\n")
        sys.stdout.flush()
    return 0


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--stdio", action="store_true")
    args = parser.parse_args()
    if not args.stdio:
        parser.error("Only --stdio transport is supported")
    return run_stdio()


if __name__ == "__main__":
    raise SystemExit(main())
