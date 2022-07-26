package mal

import (
    "fmt"
    "os"
    "strings"
    "testing"
    "github.com/xeipuuv/gojsonschema"
    "path/filepath"
)

// TestSampleShape checks the shape sample json
func TestSampleShape(t *testing.T) {
    module_path, _ := os.Getwd()

    searchDir := module_path + "/samples/"
    fmt.Println(searchDir)
    e := filepath.Walk(searchDir, func(path string, info os.FileInfo, err error) error {
        if !info.IsDir(){
            fmt.Println(path)
            relativePath := strings.Split(strings.Split(path, searchDir)[1], info.Name())[0]
            relativePath = strings.TrimRight(relativePath, "/")

            schemaLoader := gojsonschema.NewReferenceLoader("file://" + module_path + "/schemas/" + relativePath + ".json")
            documentLoader := gojsonschema.NewReferenceLoader("file://" + path)

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
        return err
    })
    
    if e != nil {
        panic(e)
    }
}