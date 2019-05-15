#To convert this to an exe use 'pyinstaller --onefile hurricane_sdk_patch.py'

import os
import tkinter as tk
from tkinter.filedialog import askdirectory
from tkinter import messagebox
import distutils.dir_util


def ConfirmSdkDirectory (dir):
  confirmed = (os.path.exists(dir) and os.path.exists(os.path.join(dir, 'api')) and 
              os.path.exists(os.path.join(dir, 'components')) and 
              os.path.exists(os.path.join(dir, 'applications')) and 
              os.path.exists(os.path.join(dir, 'hardware')) and 
              os.path.exists(os.path.join(dir, 'tools')))
  return confirmed


print ("Hurricane SDK Patch")

# Hide the tkinter root windo
root = tk.Tk()
root.withdraw()

home = os.path.expanduser("~")
gss_workspace = "gecko_os_workspace"


result = messagebox.askyesno("Hurricane SDK Patch", "This action will copy the Hurricane source code into the Gecko OS SDK on your PC. Would you like to continue?")
if result == True :
  sdk_confirm = False
  canceled = False
  
  print ("Select the path to the Gecko OS SDK.")
  while (sdk_confirm == False and canceled == False) :
    sdk_dir = askdirectory(initialdir = os.path.join(home, gss_workspace))
    if not sdk_dir :
      canceled = True
    else :
      sdk_confirm = ConfirmSdkDirectory(sdk_dir)  
      if sdk_confirm == False:
        messagebox.showinfo("Hurricane SDK Patch", "The selected path is not a Gecko OS SDK\n\n%s\n\nThe path should look similar to the following example:\n\n%s"%(sdk_dir, os.path.join(home, gss_workspace,"sdk-wgm160p_beta","4.0.17-1377")))

  if sdk_confirm == True :
    
    print("Removing old hurricane folders from the SDK if existing")
    applications_hurricane_path = os.path.join(sdk_dir,"applications","hurricane")
    if(os.path.exists(applications_hurricane_path)) :
      distutils.dir_util.remove_tree(applications_hurricane_path)

    components_hurricane_path = os.path.join(sdk_dir,"components","hurricane")
    if(os.path.exists(components_hurricane_path)) :
      distutils.dir_util.remove_tree(components_hurricane_path)

    print ("Copying files...")
    source_dir = os.path.join(os.getcwd(), "sdk-hurricane")
    copy_result = distutils.dir_util.copy_tree(source_dir, sdk_dir)
    log_file = os.path.join(sdk_dir, "Hurricane-Patch.log") 
    with open(log_file,"w") as text_file :
        text_file.write ("%s"%copy_result)
    message = "Success!\n\nFor a list of files copied to the SDK see the log at\n  %s"%(log_file)
    print (message)
    messagebox.showinfo("Hurricane SDK Patch", message)
  else :
    messagebox.showinfo("Hurricane SDK Patch", "The operation was canceled!")    
