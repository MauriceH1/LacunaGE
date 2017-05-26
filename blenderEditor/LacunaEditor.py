bl_info = {
        "name": "LacunaGE Editor",
        "category": "LacunaGE",
}

# exporter link
# https://github.com/jlamarche/iOS-OpenGLES-Stuff/blob/master/Blender%20Export/objc_blend_2.62/io_export_objective_c_header.py

import bpy,math
from bpy.types import Panel,Operator
from math import pi

# Custom operator for adding entities, derived from Operator
class LCN_Add_Entity(Operator):
    """add Lacuna entity"""
    bl_idname = "lacuna.add_entity"
    bl_label = "Instantiates an entity"
    bl_options = {'REGISTER', 'UNDO'}
    
    def execute(self, context):
        bpy.ops.object.empty_add(type='ARROWS', view_align=False, location=(0, 0, 0))
        return {'FINISHED'}

# Custom operator for adding lights, derived from Operator
class LCN_Add_Light(Operator):
    """add Lacuna light"""
    bl_idname = "lacuna.add_light"
    bl_label = "Instantiates a light"
    bl_options = {'REGISTER', 'UNDO'}
    
    def execute(self, context):
        bpy.ops.object.empty_add(type='PLAIN_AXES', view_align=False, location=(0, 0, 0))
        return {'FINISHED'}

# Custom operator for adding cameras, derived from Operator
class LCN_Add_Camera(Operator):
    """add Lacuna Camera"""
    bl_idname = "lacuna.add_camera"
    bl_label = "Instantiates a camera"
    bl_options = {'REGISTER', 'UNDO'}
    
    def execute(self, context):
        bpy.ops.object.camera_add(view_align=True, enter_editmode=False, location=(0, 0, 0), rotation=(pi / 2, 0, 0))
        return {'FINISHED'}

# Class for the panel, derived from Panel
class LacunaToolPanel(Panel):
    bl_space_type = 'VIEW_3D'
    bl_region_type = 'TOOLS'
    bl_label = 'Lacuna Tools'
    bl_context = 'objectmode'
    bl_category = 'LacunaGE'
    
    # Draw UI elements here
    def draw(self, context):
        layout = self.layout
        layout.operator('lacuna.add_entity', text='Add Entity')
        layout.operator('lacuna.add_camera', text='Add Camera')
        layout.operator('lacuna.add_light', text='Add Light')
    
# Register Addon
def register():
    bpy.utils.register_class(LCN_Add_Light)
    bpy.utils.register_class(LCN_Add_Camera)
    bpy.utils.register_class(LCN_Add_Entity)
    bpy.utils.register_class(LacunaToolPanel)

# Unregister Addon
def unregister():
    bpy.utils.unregister_class(LacunaToolPanel)
    bpy.utils.unregister_class(LCN_Add_Entity)
    bpy.utils.unregister_class(LCN_Add_Camera)
    bpy.utils.unregister_class(LCN_Add_Light)

# Needed to run from text editor
if __name__ == '__main__':
    register()