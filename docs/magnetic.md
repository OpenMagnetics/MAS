# Magnetic
I am going to separate into different specifications (and therefore files or entries in the database) anything that has to do with input to the system (operating points and requirements) from the physical system itself (the magnetic component). This first chapter describes how a magnetic can be described.

It is a common practice of magnetics manufacturers to decouple cores (including gaps) and windings, having collections of compatible cores and wound bobbins, which allows having a multitude of different magnetics by combining them.

I have followed the same principle for separating the different parts of the magnetic component, choosing to define the core and winding as two (almost) independent objects. Cores and windings can be defined by themselves and linked together, supporting even having magnetic cores with more than one winding window, and placing different windings in each one.

## Name
This name references the magnetic component and can be used to refer to it. This field can contain any valid string of characters, and can hold the manufacturer's reference or a description of a custom magnetic.

## Core
As defined in the [Core Section] (https://github.com/OpenMagnetics/MAS/blob/main/docs/magnetic/core.md)

## Coil
As defined in the [Coil Section] (https://github.com/OpenMagnetics/MAS/blob/main/docs/magnetic/coil.md)

```mermaid
classDiagram

class Magnetic {
    
    -String name
    -MagneticCore core
    -Coil coil
    -List<DistributorInfo> distributors_info;
    -MagneticManufacturerInfo manufacturer_info;

    +get_*()
    +set_*()
    
}

Magnetic ..> MagneticCore : Dependency
Magnetic ..> Coil : Dependency
Magnetic ..> DistributorInfo : Dependency
Magnetic ..> MagneticManufacturerInfo : Dependency

class MagneticCore {

    -String name
    -FunctionalDescription functional_description
    -List~Piece~ geometrical_description
    -ProcessedDescription processed_description
    -ManufacturingInfo manufacturing_info

    +process_data()
    +process_gap()
    +get_*()
    +set_*()
}

class Coil {
    -Bobbin bobbin;
    -List~CoilFunctionalDescription~ functional_description;
    -List~Layer~ layers_description;
    -List~Section~ sections_description;
    -List~Turn~ turns_description;

    +wind_by_sections()
    +wind_by_layers()
    +wind_by_turns()
    +get_*()
    +set_*()
}

class MagneticManufacturerInfo {
    -String cost
    -String datasheet_url
    -String name
    -String reference
    -String status
    -MagneticManufacturerRecommendations recommendations;

    +get_*()
    +set_*()
}
class DistributorInfo {
    -Double cost;
    -String country;
    -String distributed_area;
    -String email;
    -String link;
    -String name;
    -String phone;
    -Double quantity;
    -String reference;
    -String updated_at;

    +get_*()
    +set_*()
}
MagneticManufacturerInfo ..> MagneticManufacturerRecommendations : Dependency

class MagneticManufacturerRecommendations {
    -Double rated_current;
    -Double rated_current_temperature_rise;
    -Double rated_magnetic_flux;

    +get_*()
    +set_*()
}

```
