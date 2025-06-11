# pip install libclang
import clang.cindex as CX
import os
from datetime import datetime


def generate_enum_to_string(enum: CX.Cursor):
    # 检查是否是系统头文件中的枚举
    if enum.location.file and 'include' in enum.location.file.name:
        return ""
    
    branchs = ""
    for child in enum.get_children():
        # 检查是否是 enum class
        is_enum_class = enum.is_scoped_enum()
        enum_type = enum.spelling
        if is_enum_class:
            # 对于 enum class，需要显式类型转换 - case static_cast<ColorClass>(0): return "RED";
            branchs += f'case static_cast<{enum_type}>({child.enum_value}): return "{child.spelling}";\n'
        else:
            # 对于普通 enum，直接使用数值 - case 0: return "RED";
            branchs += f'case {child.enum_value}: return "{child.spelling}";\n'
        
    code = f"""
// Auto-generated enum to string conversion function
// Generated at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
std::string_view {enum.spelling}_to_string({enum.spelling} value) {{
    switch(value) {{
        {branchs}
        default: return "";
    }}
}}
"""
    return code


def traverse(node: CX.Cursor):
    result = []
    if node.kind == CX.CursorKind.ENUM_DECL:
        code = generate_enum_to_string(node)
        if code:  # 只添加非空的代码
            result.append(code)

    for child in node.get_children():
        result.extend(traverse(child))
    
    return result


def main():
    index = CX.Index.create()
    this_dir = os.path.dirname(os.path.abspath(__file__))
    file_name = "enum02.cpp"
    tu = index.parse(os.path.join(this_dir, file_name))

    # Collect all generated code
    generated_code = traverse(tu.cursor)

    # Add file header comments
    header = f"""// This file is auto-generated, do not modify manually
// Generated at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
// Source file: {file_name}

"""
    
    # Write to file
    output_file = os.path.join(this_dir, "enum02_autogen.inc")
    with open(output_file, 'w') as f:
        f.write(header)
        f.write('\n'.join(generated_code))
    
    print(f"Generated file: {output_file}")


if __name__ == "__main__":
    main()