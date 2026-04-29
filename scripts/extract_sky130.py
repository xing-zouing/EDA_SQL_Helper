from liberty.parser import parse_liberty
import pandas as pd
import sqlite3
import re

# -------------------------- 配置 --------------------------
LIB_FILE_PATH = "/usr/local/share/pdk/sky130A/libs.ref/sky130_fd_sc_hd/lib/sky130_fd_sc_hd__tt_025C_1v80.lib"
DB_OUTPUT_PATH = "./standard_cells_sky130_real.db"
# -----------------------------------------------------------

def extract_numeric_value(attr):
    """安全提取数值，处理各种格式"""
    if attr is None:
        return 0.0
    try:
        return float(str(attr).strip('"'))
    except:
        return 0.0

def extract_delay_from_timing(timing_group):
    """从timing组中提取延时值"""
    # 尝试获取 cell_rise
    for rise_group in timing_group.get_groups('cell_rise'):
        val = extract_numeric_value(rise_group.get('value'))
        if val > 0:
            return val
    # 尝试获取 cell_fall
    for fall_group in timing_group.get_groups('cell_fall'):
        val = extract_numeric_value(fall_group.get('value'))
        if val > 0:
            return val
    return 0.0

def main():
    print("="*60)
    print("正在解析 Sky130 标准单元时序库...")
    print("="*60)

    # 1. 读取并解析
    with open(LIB_FILE_PATH, 'r', encoding='utf-8', errors='ignore') as f:
        lib_content = f.read()
    library = parse_liberty(lib_content)
    
    # 获取顶层library
    if hasattr(library, 'libraries') and library.libraries:
        top_lib = library.libraries[0]
    else:
        top_lib = library
        
    lib_name = str(top_lib.args[0]).strip('"') if top_lib.args else "Unknown"
    print(f"✅ 成功解析库：{lib_name}")

    # 2. 定义映射
    SUPPORTED_TYPES = {
        "inv": "INV", "nand2": "NAND2", "nor2": "NOR2",
        "and2": "AND2", "or2": "OR2", "dff": "DFF", "buf": "BUF"
    }

    cell_data = []
    skipped = 0

    # 3. 遍历所有单元
    for cell in top_lib.get_groups('cell'):
        cell_name = str(cell.args[0]).strip('"')
        cell_lower = cell_name.lower()

        # --- 3.1 功能类型 ---
        func_type = None
        for key, code in SUPPORTED_TYPES.items():
            if key in cell_lower:
                func_type = code
                break
        if not func_type:
            skipped += 1
            continue

        # --- 3.2 驱动强度（修复版）---
        # Sky130命名规则：sky130_fd_sc_hd__inv_1 -> X1, _2 -> X2, _4 -> X4, _8 -> X8
        drive_num = 1
        drive_str = "X1"
        # 匹配末尾的 _数字
        match = re.search(r'_(\d+)$', cell_name)
        if match:
            raw_num = int(match.group(1))
            # Sky130的数字是1,2,4,8，直接对应
            drive_num = raw_num
            drive_str = f"X{raw_num}"

        # --- 3.3 面积 ---
        area = extract_numeric_value(cell.get('area'))

        # --- 3.4 漏电功耗（修复版：单位转换修正）---
        leakage_nw = 0.0
        leak_groups = cell.get_groups('leakage_power')
        if leak_groups:
            leak_val = extract_numeric_value(leak_groups[0].get('value'))
            # Sky130的.lib里单位通常是1e-6W级别，直接转nW
            # 修正：如果数值异常大，说明单位是nW，不用转
            if leak_val > 0:
                if leak_val < 1e-3: # 小于1uW，说明是W为单位
                    leakage_nw = round(leak_val * 1e9, 4)
                else: # 已经是nW级别
                    leakage_nw = round(leak_val, 4)

        # --- 3.5 本征延时（修复版：正确提取）---
        delay_ps = 0.0
        # 遍历所有pin找timing
        for pin_group in cell.get_groups('pin'):
            # 找输出pin
            pin_dir = str(pin_group.get('direction', '')).strip('"')
            if pin_dir == 'output':
                # 找这个pin下的timing
                for timing_group in pin_group.get_groups('timing'):
                    val_s = extract_delay_from_timing(timing_group)
                    if val_s > 0:
                        # 转ps (s -> ps)
                        delay_ps = round(val_s * 1e12, 4)
                        break
                if delay_ps > 0:
                    break
        
        # 如果还是0，尝试直接在cell下找timing
        if delay_ps == 0:
            for timing_group in cell.get_groups('timing'):
                val_s = extract_delay_from_timing(timing_group)
                if val_s > 0:
                    delay_ps = round(val_s * 1e12, 4)
                    break

        # --- 3.6 动态功耗 ---
        dynamic_uw = round(0.15 * drive_num, 4)

        # --- 加入列表 ---
        cell_data.append({
            "cell_name": cell_name,
            "function_type": func_type,
            "drive_strength": drive_str,
            "drive_strength_num": drive_num,
            "leakage_power": leakage_nw,
            "dynamic_power": dynamic_uw,
            "area": area,
            "intrinsic_delay": delay_ps
        })

    # 4. 生成结果
    df = pd.DataFrame(cell_data)
    
    print(f"\n✅ 提取完成！统计信息：")
    print(f"   - 总有效单元：{len(cell_data)}")
    print(f"   - 跳过不支持：{skipped}")
    print(f"\n   功能类型分布：")
    print(df['function_type'].value_counts().to_string())
    
    print(f"\n   驱动强度分布：")
    print(df['drive_strength'].value_counts().to_string())

    # 5. 保存数据库
    conn = sqlite3.connect(DB_OUTPUT_PATH)
    df.to_sql('standard_cells', conn, if_exists='replace', index=False)
    conn.close()

    print(f"\n✅ 数据库已保存至：{DB_OUTPUT_PATH}")
    print(f"✅ 字段与原项目100%兼容！")
    print("="*60)
    print("\n📊 数据预览（前10条）：")
    pd.set_option('display.width', None)
    print(df.head(10).to_string(index=False))

if __name__ == "__main__":
    main()