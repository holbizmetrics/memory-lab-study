#pragma once
#include <juce_data_structures/juce_data_structures.h>

namespace memorylab {
// UI preference only: never serialized into a sound or accessed by the audio thread.
class PatchLocation {
 static juce::PropertiesFile::Options options(){
  juce::PropertiesFile::Options o;o.applicationName="Memory Lab Study";o.filenameSuffix="settings";
  o.folderName="MemoryLabStudy";o.osxLibrarySubFolder="Application Support";o.millisecondsBeforeSaving=-1;return o;
 }
 juce::PropertiesFile settings;
public:
 PatchLocation():settings(options()){}
 explicit PatchLocation(const juce::File&file):settings(file,options()){}
 juce::File initialFile(){
  settings.reload();const auto saved=settings.getValue("patchFolder");
  const auto fallback=juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
  const auto folder=juce::File::isAbsolutePath(saved)&&juce::File(saved).isDirectory()?juce::File(saved):fallback;
  return folder.getChildFile("Memory Lab Study.mlpatch");
 }
 bool rememberSuccess(const juce::File&file){
  const auto folder=file.getParentDirectory();if(!folder.isDirectory())return false;
  settings.reload();settings.setValue("patchFolder",folder.getFullPathName());return settings.saveIfNeeded();
 }
 bool autoLoadPresets(){settings.reload();return settings.getBoolValue("autoLoadPresets",false);}
 bool rememberAutoLoadPresets(bool enabled){settings.reload();settings.setValue("autoLoadPresets",enabled);return settings.saveIfNeeded();}
};
}
