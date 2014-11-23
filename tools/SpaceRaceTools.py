bl_info = {
    "name": "SpaceRace tools",
    "author": "Timothy Costigan <costigt@tcd.com>",
    "version": (0,1),
    "blender": (2,7,2),
    "location": "",
    "description": "SpaceRace map editing tools",
    "warning": "",
    "wiki_url": "",
    "tracker_url": "",
    "category": "Object"}

import bpy
from bpy_extras.io_utils import ExportHelper
from xml.dom.minidom import Document

bpy.types.Object.SR_setup_script = bpy.props.StringProperty(name = "Setup Script", subtype='FILE_PATH')
bpy.types.Object.SR_update_script = bpy.props.StringProperty(name = "Update Script", subtype='FILE_PATH')
bpy.types.Object.SR_interact_script = bpy.props.StringProperty(name = "Interact Script", subtype='FILE_PATH')
bpy.types.Object.SR_model = bpy.props.StringProperty(name = "Model", subtype = 'FILE_PATH')

bpy.types.Object.SR_mass = bpy.props.FloatProperty(name = "Mass", default = 0.0, min = 0.0)

class SpaceRaceObjectPanel(bpy.types.Panel):
    bl_label = "SpaceRace Object Properties"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "object"

    def draw(self, context):
        layout = self.layout

        object = context.active_object
        
        layout.prop(object,"SR_model")
        
        layout.separator()
        
        layout.prop(object,"SR_setup_script")
        layout.prop(object,"SR_update_script")
        layout.prop(object,"SR_interact_script")
        layout.prop(object,"SR_mass")
        
bpy.types.World.SR_setup_script = bpy.props.StringProperty(name = "Setup Script", subtype='FILE_PATH')
bpy.types.World.SR_update_script = bpy.props.StringProperty(name = "Update Script", subtype='FILE_PATH')  

class SpaceRaceWorldPanel(bpy.types.Panel):
    bl_label = "SpaceRace World Properties"
    bl_space_type = 'PROPERTIES'
    bl_region_type = 'WINDOW'
    bl_context = "world"
    COMPAT_ENGINES = {'BLENDER_RENDER','BLENDER_GAME'}

    def draw(self, context):
        layout = self.layout
        
        world = context.world
        layout.prop(world,"SR_setup_script")
        layout.prop(world,"SR_update_script")

def convertVec(vec,flip):
    if flip:
        return vec.x,vec.y,vec.z
    else:
        return vec.y,vec.z,vec.x
    
def convertQuat(quat,flip):
    if flip:
        return quat.w,quat.x,quat.y,quat.z
    else:
        return quat.w,quat.y,quat.z,quat.x
        
class SpaceRaceExporter(bpy.types.Operator,ExportHelper):
    bl_idname = "sr_export.xml"
    bl_label = "Export SpaceRace level .xml"
    filename_ext = ".xml"
    
    filter_glob = bpy.props.StringProperty(default="*.xml",options={'HIDDEN'})
    
    axis_flip = bpy.props.BoolProperty(name = "Use OpenGL Coord System", default = True)
    
    def execute(self,context):
        
        doc = Document()
        
        world = bpy.data.worlds['World']
        
        map = doc.createElement("world")
        map.setAttribute("setup",world.SR_setup_script)
        map.setAttribute("update",world.SR_update_script)
        
        for obj in bpy.data.objects:
            #determine type
            objType = obj.type
            obj.rotation_mode = 'QUATERNION'
            pX,pY,pZ = convertVec(obj.location, self.axis_flip)
            rW,rX,rY,rZ = convertQuat(obj.rotation_quaternion, self.axis_flip)
            
            mesh = doc.createElement("entity")
            mesh.setAttribute("name",obj.name)
            mesh.setAttribute("model",obj.SR_model)
            mesh.setAttribute("setup",obj.SR_setup_script)
            mesh.setAttribute("update",obj.SR_update_script)
            mesh.setAttribute("interact",obj.SR_interact_script)
            mesh.setAttribute("mass",str(obj.SR_mass))
                
            #position and rotation
            pString = str(pX) + "," + str(pY) + "," + str(pZ)
            qString = str(rW) + "," + str(rX) + "," + str(rY) + "," + str(rZ)
                
            mesh.setAttribute("position",pString)
            mesh.setAttribute("rotation",qString)
            map.appendChild(mesh)
        
        #export
        doc.appendChild(map)
        file = open(self.filepath,'w')
        file.write(doc.toprettyxml())
        file.close()
        
        return {'FINISHED'}
    
def SR_menu_func_export(self,context):
    self.layout.operator(SpaceRaceExporter.bl_idname, text="SpaceRace Export (.xml)")

def register():
    bpy.utils.register_class(SpaceRaceObjectPanel)
    bpy.utils.register_class(SpaceRaceWorldPanel)
    bpy.utils.register_class(SpaceRaceExporter)
    bpy.types.INFO_MT_file_export.append(SR_menu_func_export)

def unregister():
    bpy.utils.unregister_class(SpaceRaceObjectPanel)
    bpy.utils.unregister_class(SpaceRaceWorldPanel)
    bpy.utils.unregister_class(SpaceRaceExporter)
    bpy.types.INFO_MT_file_export.remove(SR_menu_func_export)


if __name__ == "__main__":
    register()
