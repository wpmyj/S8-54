import os
import datetime

os.system('del sources\\*.d /s /q')
os.system('del sources\\*.bak /s /q')
os.system('del sources\\*.o /s /q')
os.system('del sources\\*.ncb /s /q')
os.system('del sources\\*.sdf /s /q')
os.system('del sources\\*.pch /s /q')
os.system('del sources\\S8-53\\Listings\\*.* /s /q')
os.system('del sources\\S8-53\\Objects\\*.* /s /q')
os.system('del sources\\S8-54\\Listings\\*.* /s /q')
os.system('del sources\\S8-54\\Objects\\*.* /s /q')
os.system('del sources\\VS-OSCI\\S8-53\\Debug\\*.* /s /q')
os.system('del sources\\VS-OSCI\\S8-53\\ARM\\*.* /s /q')
os.system('del sources\\VS-OSCI\\S8-54\\Debug\\*.* /s /q')
os.system('del sources\\VS-OSCI\\S8-54\\ARM\\*.* /s /q')
os.system('del sources\\VS-OSCI\\ControllerETH\bin\\*.* /s /q')
os.system('del sources\\VS-OSCI\\ControllerETH\obj\\*.* /s /q')
os.system('del sources\\VS-OSCI\\ControllerUSB\bin\\*.* /s /q')
os.system('del sources\\VS-OSCI\\ControllerUSB\obj\\*.* /s /q')


date = datetime.datetime.now()

name = 'Archiv\\S8-53-54' + date.strftime("_%Y_%m_%d_%H_%M_%S") + '.zip'
os.system('7z a ' + name + ' sources')
os.system('copy ' + name + ' h:work\\')
os.system('copy ' + name + ' g:\\')

os.system('pause')
