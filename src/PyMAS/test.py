import pprint
import PyMAS

coreData = {
    'functionalDescription': {
        'bobbin': None,
        'gapping': [],
        'material': '3C97',
        'name': 'default',
        'numberStacks': 1,
        'shape': {'aliases': [],
                  'dimensions': {'A': 0.0308,
                                 'B': 0.0264,
                                 'C': 0.0265,
                                 'D': 0.016,
                                 'E': 0.01,
                                 'G': 0.0,
                                 'H': 0.0},
                  'family': 'u',
                  'familySubtype': '1',
                  'name': 'Custom',
                  'type': 'custom'},
        'type': 'two-piece set'
    }
}

# shapeData = {'type': 'standard', 'family': 'u', 'aliases': [], 'name': 'U 126/91/20', 'dimensions': {'A': 0.126, 'B': 0.09100000000000001, 'C': 0.02, 'E': 0.068, 'D': 0.063}, 'familySubtype': None}
# coreData = {'functionalDescription': {'bobbin': None, 'gapping': [], 'material': '3C97', 'shape': {'family': 'pm', 'type': 'custom', 'aliases': [], 'dimensions': {'A': 0.1118, 'B': 0.046299999999999994, 'C': 0.045, 'D': 0.0319, 'E': 0.08979999999999999, 'F': 0.0286, 'G': 0.052, 'H': 0.0056, 'b': 0.0058, 't': 0.004200000000000001}, 'familySubtype': '2', 'name': 'Custom'}, 'type': 'two-piece set', 'name': 'default', 'numberStacks': 1}, 'geometricalDescription': None, 'processedDescription': None}
# piece_data = PyMAS.get_piece_data(shapeData)
# pprint.pprint(piece_data)
core_data = PyMAS.get_core_data(coreData)
pprint.pprint(core_data)
