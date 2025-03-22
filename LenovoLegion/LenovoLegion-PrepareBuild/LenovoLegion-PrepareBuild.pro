TEMPLATE        = lib
TARGET          = __prepare-build_none__

QMAKE_POST_LINK += $$quote($${SYSTEM_MKDIR} -p $${DESTINATION_PATH}$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote($${SYSTEM_MKDIR} -p $${DESTINATION_DATA_PATH}$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote($${SYSTEM_MKDIR} -p $${DESTINATION_BIN_PATH}$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote($${SYSTEM_MKDIR} -p $${DESTINATION_LIB_PATH}$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote($${SYSTEM_MKDIR} -p $${DESTINATION_LOG_PATH}$$escape_expand(\\n\\t))
QMAKE_POST_LINK += $$quote($${SYSTEM_MKDIR} -p $${DESTINATION_MODULES_PATH}$$escape_expand(\\n\\t))
