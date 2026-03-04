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
- Built robust development framework using CMake, including custom types for geometry handling
- Developed pinhole camera model with multithreaded renderer and anti-aliasing for smooth generation
- Implemented multi-object scene framework supporting Lambertian shading, point lights, and shadow ray casting

Planned next steps include:
- Implement material models
- Implement gamma correction
- Implement triangle geometry
- Address BVH acceleration for performance

## Preliminary Outputs

Representative intermediate outputs include:
- Normal gradient shperes rendering
- Masking test spheres rendering
- Lambert shader spheres rendering with anti-aliasing

## Repository Structure

- 'src/' - C++ implementation
- 'figures/' - Normal, masking, and lambert gradient test .ppm results