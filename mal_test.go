package mal

import (
    "fmt"
    "os"
    "strings"
    "testing"
    "github.com/xeipuuv/gojsonschema"
    "path/filepath"
    "io"
    "encoding/json"
)

func CheckJson(t *testing.T, schema string, jsonToCheck interface{}) {
    var documentLoader gojsonschema.JSONLoader

    switch v := jsonToCheck.(type) {
        case string:
            documentLoader = gojsonschema.NewReferenceLoader(v)
        case map[string]interface{}:
            documentLoader = gojsonschema.NewGoLoader(v)
    }

    schemaLoader := gojsonschema.NewReferenceLoader(schema)

    result, err := gojsonschema.Validate(schemaLoader, documentLoader)
    if err != nil {
        t.Errorf(err.Error())
    }

    if result.Valid() {
        fmt.Printf("The document is valid\n")
    } else {
        t.Errorf("The document is not valid. see errors above\n")
        for _, desc := range result.Errors() {
            fmt.Printf("- %s\n", desc)
        }
    }
}

// TestSampleShape checks sample jsons
func TestSampleShape(t *testing.T) {
    module_path, _ := os.Getwd()

    searchDir := module_path + "/samples/"
    fmt.Println(searchDir)
    e := filepath.Walk(searchDir, func(path string, info os.FileInfo, err error) error {
        if !info.IsDir(){
            fmt.Println(path)
            relativePath := strings.Split(strings.Split(path, searchDir)[1], info.Name())[0]
            relativePath = strings.TrimRight(relativePath, "/")

            CheckJson(t, "file://" + module_path + "/schemas/" + relativePath + ".json", "file://" + path)

        }
        return err
    })
    
    if e != nil {
        panic(e)
    }
}

// TestShapesData checks the shapes data
func TestShapesData(t *testing.T) {
    module_path, _ := os.Getwd()

    dataFile := module_path + "/data/shapes.ndjson"

    // open the file pointer
    shapesFile, err := os.Open(dataFile)
    if err != nil {
        panic(err)
    }
    defer shapesFile.Close()

    // create a new decoder
    var shapesDecoder *json.Decoder = json.NewDecoder(shapesFile)
    if err != nil {
        panic(err)
    }

    for {
        var doc map[string]interface{}

        err := shapesDecoder.Decode(&doc)
        if err == io.EOF {
            // all done
            break
        }
        if err != nil {
            panic(err)
        }

        fmt.Printf("name: %+v\n", doc["name"])
        CheckJson(t, "file://" + module_path + "/schemas/core/shape.json", doc)

    }
}

// TestBobbinsData checks the bobbins data
func TestBobbinsData(t *testing.T) {
    module_path, _ := os.Getwd()

    dataFile := module_path + "/data/bobbins.ndjson"

    // open the file pointer
    bobbinsFile, err := os.Open(dataFile)
    if err != nil {
        panic(err)
    }
    defer bobbinsFile.Close()

    // create a new decoder
    var bobbinsDecoder *json.Decoder = json.NewDecoder(bobbinsFile)
    if err != nil {
        panic(err)
    }

    for {
        var doc map[string]interface{}

        err := bobbinsDecoder.Decode(&doc)
        if err == io.EOF {
            // all done
            break
        }
        if err != nil {
            panic(err)
        }

        fmt.Printf("name: %+v\n", doc["name"])
        CheckJson(t, "file://" + module_path + "/schemas/core/bobbin.json", doc)

    }
}