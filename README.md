# Tiny Renderer

## Overview

This repository contains an ongoing project focused on the implementation of a simple image renderer employing simple ray tracing and threading.

## Methodology

The project follows the given methodology:
- Point light model
- Lambert shading

## Current Status

This project is a work in progress.
At the current stage:
- Defined project structure
- Implemented CMake builder
- Defined vector class
- Defined custom class operators
- Included simple ray class
- Implementd RGB image binary output stream
- Implementd ray sphere hit test
- Implementd simple pinhole camera model
- Implementd simple gradient renderer
- Implementd threading for code optimization, increasing code efficienty by 3 fold
- Defined DCM class, updated camera orientation representation

Planned next steps include:
- Validate pinhole camera model
- Implement proper shading pipeline
- Implement material models

## Preliminary Outputs

Representative intermediate outputs include:
- Gradient shaded sphere rendering

## Repository Structure

- 'src/' - C++ implementation
- 'figures/' - Preliminary .ppm outputs