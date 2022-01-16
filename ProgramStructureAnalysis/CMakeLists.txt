# Count
add_subdirectory(CountCall) # CountBranch
#add_subdirectory(CountBranch) # CountBranch
#add_subdirectory(CountStaticCircle)


# Get Index
#add_subdirectory(GetDynamicCircleIndex) # Get Dynamic Circle

# Detector

add_subdirectory(LoopDetector) # Detect Loop Info
add_subdirectory(ProgramStructureDetector) # Detector Program Structure

# Get Info
add_subdirectory(GetBBInfoInOuterLoop)
add_subdirectory(GetBBInfoInAllLoop)
add_subdirectory(GetBBLLFIInfoInOuterLoop)
add_subdirectory(GetBBLLFIInfoInAllLoop)


#Get Name
add_subdirectory(GetBBName)

#Global Checker Optimizer
add_subdirectory(GlobalCheckerOptimizer)