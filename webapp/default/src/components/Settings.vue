<template>
  <v-container>
    <v-toolbar>
      <v-app-bar-nav-icon @click="nav2page('Home')">
        <v-icon>mdi-arrow-left-circle-outline</v-icon>
      </v-app-bar-nav-icon>

      <v-toolbar-title>{{ $t("main_title") }} &#x2022; {{ $t("settings_header") }} {{ user_info.username
        }}</v-toolbar-title>
    </v-toolbar>
    <!--v-container-->
    <v-card class="mx-auto">
      <v-form ref="form">
        <v-btn text @click="nav2page('Profile')">
          {{ $t('settings_profile') }}
        </v-btn>
        <v-btn text @click="nav2page('DeviceAdd')">
          {{ $t('settings_devices') }}
        </v-btn>
      </v-form>

    </v-card>
  </v-container>
</template>

<script>
// import messenger from "../messenger";
import router from "../router";

export default {
  name: 'Settings',
  components: {
  },
  data() {
    return {

    }
  },
  methods: {

    nav2page(page) {
      router.push({ name: page, params: { user_info: this.user_info } });
    },

  },
  created() {
    this.user_info = this.$route.params.user_info
    console.log("user_info", this.user_info)
  },
  mounted() {

  },
  watch: {

  }
}
</script>
