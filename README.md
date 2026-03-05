# Tiny Renderer

## Overview

This repository contains an ongoing project focused on the implementation of a minimal ray tracing renderer written in modern C++. The goal is to progressively build a physics-based rendering pipeline while maintaining clear and compact code suitable for experimentation and learning.

The renderer currently supports basic geometry, materials, recursive ray tracing, and directional lighting, forming the foundation of a small path-tracing style renderer.

## Methodology

The renderer currently implements the following core components:

- Directional light model (sun-like illumination)
- Recursive ray tracing with configurable bounce depth
- Lambertian diffuse materials
- Perfect specular metal reflection
- Hard shadow casting via shadow rays
- Pinhole camera model
- Anti-aliasing through stochastic sampling
- Reinhard tone mapping
- Gamma correction for display output
- Multithreaded tile-based rendering

## Current Status

This project is a work in progress.
At the current stage:

- A modular C++ architecture with custom math types (vec3, ray, col3) and geometry interfaces
- A multithreaded rendering framework using CMake
- A pinhole camera model generating primary rays for each pixel
- Anti-aliasing via stochastic pixel sampling
- A multi-object scene framework using a hittable interface and scene container
- Metal material model with perfect mirror reflection
- Recursive ray tracing enabling multi-bounce light transport
- Directional light source representing solar illumination
- Hard shadow testing using shadow rays
- HDR radiance accumulation with Reinhard tone mapping and gamma correction

The renderer now supports indirect illumination through recursive ray scattering, allowing colored reflections and light transport between objects.

## Planned Next Steps

Planned next steps include:
- Triangle mesh geometry support
- BVH acceleration structures for faster intersection queries
- Cosine-weighted hemisphere sampling for improved diffuse convergence
- Next Event Estimation to reduce noise in shadowed regions
- Additional BRDF material models

## Preliminary Outputs

Representative intermediate outputs include:
- Lambertian diffuse shading with directional lighting
- Hard shadow casting
- Recursive reflections between objects
- Anti-aliased renders with HDR tone mapping

Example scene demonstrates indirect illumination effects such as colored reflections between diffuse objects and specular surfaces.

## Repository Structure

- 'src/' - C++ implementation
- 'figures/' - Example render .ppm results