import sys, getopt
from xml.etree.ElementTree import parse, Element

if __name__ == "__main__":
    path=None
    build_snippets=True
    build_public_samples=True
    generate_static_library=False
    use_static_wincrt=True
    use_debug_wincrt=True
    float_point_precise_math=False

    opts = getopt.getopt(sys.argv[1:],'-h',['help', 'path=', 'build_snippets=', 
        'build_public_samples=', 'generate_static_library=',
        'use_static_wincrt=', 'use_debug_wincrt=', 'float_point_precise_math='])

    # print(opts[0])
    for opt_name,opt_value in opts[0]:
        if opt_name in ('-h','--help'):
            print("[*] Help info")
            continue
        if opt_name in ('--path',):
            path = opt_value
            continue
        if opt_name in ('--build_snippets',):
            if opt_value.lower() == 'true':
                build_snippets = True
            else:
                build_snippets = False
            continue
        if opt_name in ('--build_public_samples',):
            if opt_value.lower() == 'true':
                build_public_samples = True
            else:
                build_public_samples = False
            continue

        if opt_name in ('--generate_static_library',):
            if opt_value.lower() == 'true':
                generate_static_library = True
            else:
                generate_static_library = False
            continue
        if opt_name in ('--use_static_wincrt',):
            if opt_value.lower() == 'true':
                use_static_wincrt = True
            else:
                use_static_wincrt = False
            continue

        if opt_name in ('--use_debug_wincrt',):
            if opt_value.lower() == 'true':
                use_debug_wincrt = True
            else:
                use_debug_wincrt = False
            continue

        if opt_name in ('--float_point_precise_math',):
            if opt_value.lower() == 'true':
                float_point_precise_math = True
            else:
                float_point_precise_math = False
            continue
    doc = parse(path)
    root = doc.getroot()

    element = root.findall('.//cmakeSwitch[@name=\'PX_BUILDSNIPPETS\']')
    element = element[0]
    if build_snippets:
        element.attrib['value']='True'
    else:
        element.attrib['value']='False'
    
    element = root.findall('.//cmakeSwitch[@name=\'PX_BUILDPUBLICSAMPLES\']')
    element = element[0]
    if build_public_samples:
        element.attrib['value']='True'
    else:
        element.attrib['value']='False'

    
    element = root.findall('.//cmakeSwitch[@name=\'PX_GENERATE_STATIC_LIBRARIES\']')
    element = element[0]
    if generate_static_library:
        element.attrib['value']='True'
    else:
        element.attrib['value']='False'

    element = root.findall('.//cmakeSwitch[@name=\'NV_USE_STATIC_WINCRT\']')
    element = element[0]
    if use_static_wincrt:
        element.attrib['value']='True'
    else:
        element.attrib['value']='False'

    element = root.findall('.//cmakeSwitch[@name=\'NV_USE_DEBUG_WINCRT\']')
    element = element[0]
    if use_debug_wincrt:
        element.attrib['value']='True'
    else:
        element.attrib['value']='False'

    element = root.findall('.//cmakeSwitch[@name=\'PX_FLOAT_POINT_PRECISE_MATH\']')
    element = element[0]
    if float_point_precise_math:
        element.attrib['value']='True'
    else:
        element.attrib['value']='False'

    doc.write(path, xml_declaration=True, encoding='utf-8')