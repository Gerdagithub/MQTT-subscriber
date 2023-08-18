include $(TOPDIR)/rules.mk

PKG_NAME:=mqtt-subscriber
PKG_VERSION:=1.0
PKG_RELEASE:=1
PKG_BUILD_DIR:=$(BUILD_DIR)/$(PKG_NAME)-$(PKG_VERSION)

include $(INCLUDE_DIR)/package.mk

define Package/mqtt-subscriber
	DEPENDS:=+libmosquitto +libuci +libTuya +libsqlite3
	SECTION:=utils
	CATEGORY:=Utilities
	TITLE:=MQTT subscriber
endef

define Package/mqtt-subscriber/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_DIR) $(1)/log
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/mqtt_subscriber $(1)/usr/bin/mqtt_subscriber
	$(INSTALL_BIN) ./files/mqtt_subscriber.init $(1)/etc/init.d/mqtt_subscriber
	$(INSTALL_BIN) ./files/mqtt_subscriber.config $(1)/etc/config/mqtt_subscriber

	$(CP) ./files/database.db $(1)/log/mqtt_subscriber_database.db
endef

$(eval $(call BuildPackage,mqtt-subscriber))