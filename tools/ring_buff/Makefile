CXX=g++
CXXFLAGS=-Wall -std=c++11 
LDFLAGS= -pthread

RELEASE_DIR=./release
DEBUG_DIR=./debug

RELEASE_CXXFLAGS=$(CXXFLAGS) -O2
DEBUG_CXXFLAGS=$(CXXFLAGS) -g

RELEASE_TARGET=$(RELEASE_DIR)/myprog
DEBUG_TARGET=$(DEBUG_DIR)/myprog

SOURCE_DIR=./
OBJECT_DIR=obj
OBJECT_DIR_DEBUG=./debug/$(OBJECT_DIR)
OBJECT_DIR_RELEASE=./release/$(OBJECT_DIR)

# 列出所有的源代码文件和头文件
SOURCES=$(wildcard $(SOURCE_DIR)/*.cpp)
HEADERS=$(wildcard $(SOURCE_DIR)/*.h)

# 将源代码文件列表转换为目标代码文件列表
OBJECTS=$(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR_DEBUG)/%.o,$(SOURCES))
OBJECTS_RELEASE=$(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR_RELEASE)/%.o,$(SOURCES))

# 自动产生的依赖关系文件
DEPENDS=$(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR_DEBUG)/%.d,$(SOURCES))
DEPENDS_RELEASE=$(patsubst $(SOURCE_DIR)/%.cpp,$(OBJECT_DIR_RELEASE)/%.d,$(SOURCES))


.PHONY: debug release clean

# 编译debug可执行文件
debug: $(OBJECT_DIR_DEBUG) $(DEBUG_TARGET)
	ln -sf $(DEBUG_TARGET) app
# 编译release可执行文件
release: $(OBJECT_DIR_RELEASE) $(RELEASE_TARGET)
	ln -sf $(RELEASE_TARGET) app
# 生成debug目标代码文件和依赖关系文件
$(OBJECT_DIR_DEBUG)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) $(DEBUG_CXXFLAGS) -MMD -MP -c $< -o $@ 

# 生成release目标代码文件和依赖关系文件
$(OBJECT_DIR_RELEASE)/%.o: $(SOURCE_DIR)/%.cpp
	$(CXX) $(RELEASE_CXXFLAGS) -MMD -MP -c $< -o $@ 

# 生成debug可执行文件
$(DEBUG_TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) -o $@ $^

# 生成release可执行文件
$(RELEASE_TARGET): $(OBJECTS_RELEASE)
	$(CXX) $(LDFLAGS) -o $@ $^

# 包含所有自动产生的依赖关系文件
-include $(DEPENDS)
-include $(DEPENDS_RELEASE)

# 清理所有中间文件和可执行文件
clean:
	rm -rf $(OBJECT_DIR_DEBUG)
	rm -rf $(OBJECT_DIR_RELEASE)
	rm -f $(DEBUG_TARGET)
	rm -f $(RELEASE_TARGET)

# 创建文件夹
$(OBJECT_DIR_DEBUG):
	mkdir -p $(OBJECT_DIR_DEBUG)
$(OBJECT_DIR_RELEASE):
	mkdir -p $(OBJECT_DIR_RELEASE)