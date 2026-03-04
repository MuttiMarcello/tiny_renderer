# Tiny Renderer

## Overview

This repository contains an ongoing project focused on the implementation of a simple image renderer employing simple ray tracing and threading.

## Methodology

The project follows the given methodology:
- Point light model
- Lambert shading
- Pinhole camera model
- Reinhart tone mapping

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
- Implemented point light class
- Implemented lambertian shading, with Reinhart tone mapping
- Implemented multiple object scene framework
- Implemented shadow ray casting
- Carried out normal gradient and making tests

Planned next steps include:
- Implement material models
- Implement gamma correction
- Implement anti-aliasing
- Implement triangles

## Preliminary Outputs

Representative intermediate outputs include:
- Normal gradient shperes rendering
- Masking test spheres rendering
- Lambert shader spheres rendering

## Repository Structure

- 'src/' - C++ implementation
- 'figures/' - Normal, masking, and lambert gradient test .ppm results