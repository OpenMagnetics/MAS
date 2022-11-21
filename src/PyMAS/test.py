import pprint
import PyMAS

# shapeData = {
#     "type": "custom",
#     "family": "pq",
#     "dimensions": {
#         "A": 0.0209,
#         "B": 0.0082,
#         "C": 0.0144,
#         "D": 0.0053,
#         "E": 0.0184,
#         "F": 0.009,
#         "G": 0.013,
#         "J": 0.0048,
#         "L": 0.0105
#     }
# }
shapeData = {'type': 'standard', 'family': 'u', 'aliases': [], 'name': 'U 126/91/20', 'dimensions': {'A': 0.126, 'B': 0.09100000000000001, 'C': 0.02, 'E': 0.068, 'D': 0.063}, 'familySubtype': None}
coreData = {'functionalDescription': {'bobbin': None,
                           'gapping': [{'area': None,
                                        'coordinates': None,
                                        'distanceClosestNormalSurface': None,
                                        'length': 0.001,
                                        'sectionDimensions': None,
                                        'shape': None,
                                        'type': 'subtractive'},
                                       {'area': None,
                                        'coordinates': None,
                                        'distanceClosestNormalSurface': None,
                                        'length': 1e-05,
                                        'sectionDimensions': None,
                                        'shape': None,
                                        'type': 'residual'}],
                           'material': '3C97',
                           'name': 'default',
                           'numberStacks': 1,
                           'shape': {'aliases': [],
                                     'dimensions': {'A': 0.126,
                                                    'B': 0.09100000000000001,
                                                    'C': 0.02,
                                                    'D': 0.063,
                                                    'E': 0.068},
                                     'family': 'u',
                                     'familySubtype': None,
                                     'name': 'U 126/91/20',
                                     'type': 'standard'},
                           'type': 'two-piece set'},
 'geometricalDescription': None,
 'processedDescription': None}
# piece_data = PyMAS.get_piece_data(shapeData)
# pprint.pprint(piece_data)
core_data = PyMAS.get_core_data(coreData)
pprint.pprint(core_data)
