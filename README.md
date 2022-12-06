Magnetic Agnostic Structure (MAS) is a project providing a common language for defining any kind of magnetic for Power Electronics, independently of its function (transformer, inductor, choke, etc.), construction (number of winding, planar or wound technology, materials used, etc.), application (power conversiob, filtering, etc.) , or manufacturer.
This is accomplished with a series of JSON schemas that provide rules for defining any magnetic.

The structure follows a modular structure, and the information is broken down in the follwing sections:
* Operation Point (IN PROGRESS)
* Operation Conditions (TBD)
* Core
* Winding (IN PROGRESS)


# Core

Complex structures, like the core, can be described in with many levels of detail, and each of these levels can be used for different purposes. 
For eexample, a simple level, containing the dimensions of the core, and the length a gap, can be used in a simple reluctance model; while a complex model like Finite Elements would need a detailed descripcion of all the pieces involved.
To this end, a "complexity level" approach has been followed, producing three different alternative levels of detail of the core: functional, geometrical, and processed level.
* Functional level: It covers the bare minimum to describe the core, but lacks enough processed information to be used in most calculations. Nevertheless, the rest of levels can be derived from this one. Additionally, the information at this level is what must be stored in a DB.
* Processed level: It contains processed information, derived from the functional level, that can be used in simple analytical models.
* Geometrical level: It contains the detailed information of how the core is made, including the different parts, and their relative position.

```mermaid
classDiagram

class Core {
    
    +FunctionalDescription functionalDescription
    
    +List~Union~Spacer, Piece~~ geometricalDescription
    +ProcessedDescription processedDescription
}
Core ..> FunctionalDescription : Dependency
Core ..> GeometricalDescription : Dependency
Core ..> ProcessedDescription : Dependency


```

```mermaid
classDiagram
class FunctionalDescription {
    +Union~CoreBobbin, String~ bobbin
    +List~CoreGap~ gapping
    +Union~CoreMaterial, String~ material
    +Union~CoreShape, String~ shape
    +FunctionalDescriptionType type
    +String name
    +Int numberStacks
}

FunctionalDescription ..> CoreBobbin : Dependency
FunctionalDescription ..> CoreGap : Dependency
FunctionalDescription ..> CoreMaterial : Dependency
FunctionalDescription ..> CoreShape : Dependency
FunctionalDescription ..> FunctionalDescriptionType : Dependency

class CoreShape {
    family: CoreShapeFamily
    +CoreBobbinType type
    +List aliases
    +Object dimensions
    +String familySubtype
    +String name
}

CoreShape ..> CoreShapeFamily : Dependency
CoreShape ..> CoreBobbinType : Dependency

class CoreShapeFamily {
    <<enumeration>>
    E
    EC
    EFD
    EL
    EP
    EPX
    EQ
    ER
    ETD
    LP
    P
    PLANAR E
    PLANAR EL
    PLANAR ER
    PM
    PQ
    RM
    U
    UI
    UR
    UT
}
class CoreBobbinType {
    <<enumeration>>
    CUSTOM
    STANDARD
}
class FunctionalDescriptionType {
    <<enumeration>>
    PIECE AND PLATE
    TOROIDAL
    TWO-PIECE SET
    CLOSED SHAPE
}

class CoreMaterial {
    +MagneticMaterialComposition composition
    +ManufacturerInfo manufacturerInfo
    +String name
    +CoreMaterialType type
    +Union~List~VolumetricLossElement~, VolumetricLossesClass~ volumetricLosses
    +List bhCycle
    +String family
    +Permeability permeability

}

class CoreBobbin {
    +CoreBobbinFamily family
    +CoreBobbinType type
    +Object dimensions
    +String familySubtype
    +ManufacturerInfo manufacturerInfo
    +String name
    +BobbinProcessedData processedData
    +String shape
}

class CoreGap {
    +Float length 
    +GappingType type
    +Float area
    +List~Float~ coordinates
    
    +Float distanceClosestNormalSurface
    +List~Float~ sectionDimensions
    +ShapeEnum shape
}

```

```mermaid
classDiagram
class ProcessedDescription {
    +List~ColumnElement~ columns
    +Float depth
    +EffectiveParameters effectiveParameters
    +Float height
    +Float width
    +List~WindingWindowElement~ windingWindows
}
ProcessedDescription ..> ColumnElement : Dependency
ProcessedDescription ..> EffectiveParameters : Dependency
ProcessedDescription ..> WindingWindowElement : Dependency


class WindingWindowElement {
    +Float area
    +List~Float~ coordinates
    +Float height
    +Float width
    +Float angle
    +Float radialHeight
}

class EffectiveParameters {
    +Float effectiveArea
    +Float effectiveLength
    +Float effectiveVolume
    +Float minimumArea
}

class ColumnElement {
    +Float area
    List~Float~ coordinates
    +Float depth
    +Float height
    +ShapeEnum shape
    +ColumnType type
    +Float width
}
ColumnElement ..> ShapeEnum : Dependency
ColumnElement ..> ColumnType : Dependency

class ShapeEnum {
    <<enumeration>>
    IRREGULAR
    OBLONG
    RECTANGULAR
    ROUND
}

class ColumnType {
    <<enumeration>>
    CENTRAL
    LATERAL
}


```
