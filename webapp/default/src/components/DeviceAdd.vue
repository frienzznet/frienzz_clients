<template>
  <v-container>
    <v-toolbar>
      <v-app-bar-nav-icon @click="nav2page('Settings')">
        <v-icon>mdi-arrow-left-circle-outline</v-icon>
      </v-app-bar-nav-icon>

      <v-toolbar-title>{{ $t("main_title") }} &#x2022; {{ $t("device_pairing") }}</v-toolbar-title>
    </v-toolbar>
    <!--v-container-->
    <v-card class="mx-auto">
      <v-form ref="form" v-model="isFormValid">
        <v-text-field minLength=4 maxLength=80 v-model="device_settings.name" :label="$t('device_pairing_device_name')"
          required :rules="[rules.required, rules.counter]"></v-text-field>
        <v-text-field maxLength=100 v-model="device_settings.qrcode" :readonly="true"
          :label="$t('device_pairing_scan_code')"></v-text-field>
        <v-btn text @click="send_device_pair_request(device_settings)" :disabled="!isFormValid" color=blue>
          <v-icon>mdi-monitor-multiple</v-icon>
          {{ $t('device_pairing_request') }}
        </v-btn>

      </v-form>
      <!--/v-container-->
      <v-dialog v-model="deviceadd_success_show" scrollable max-width="300px">
        <v-card>
          <v-card-title>{{ $t("device_pairing") }}</v-card-title>
          <v-card-text>{{ $t("device_pairing_ok_message") }}</v-card-text>
          <v-divider></v-divider>
          <v-card-actions>
            <v-btn color="blue darken-1" text @click="deviceadd_success_show = false">{{
              $t("ok") }}</v-btn>
            <!--
          <v-btn color="red darken-1" text @click="user_user_delete()">{{ $t("profile_delete_account_select") }}</v-btn>
          -->
          </v-card-actions>
        </v-card>
      </v-dialog>
    </v-card>
  </v-container>
</template>

<script>
import messenger from "../messenger";
import router from "../router";


export default {
  name: 'DeviceAdd',
  components: {
  },
  data() {
    return {
      isFormValid: false,
      deviceadd_success_show: false,
      device_settings: { "name": "", "qrcode": "" },
      rules: {
        required: value => !!value || this.$t('edit_min_4_chars'),
        counter: value => value.length >= 5 || this.$t('profile_min_chars'),
      },
    }
  },
  methods: {
    nav2page(page) {
      router.push({ name: page, params: { user_info: this.user_info } });
    },
    send_device_pair_request() {
      messenger.emit("device_pair_request", {
        device_settings: this.device_settings
      });
      this.nav2page('Home');
      //this.deviceadd_success_show = true
    },
    check_valid_data() {
      if (this.device_settings.name.length < 4 || this.device_settings.qrcode.length < 5) {
        return
      }
      return true
    },
  },
  created() {
    this.user_info = this.$route.params.user_info
    console.log("user_info", this.user_info)
    if (localStorage.userName && localStorage.token) {
      var token = localStorage.token;
      messenger.init(token);
      //messenger.emit("home_status", {}); // to set up everything for the home page, including user_info
    } else {
      this.nav2page("Login");
    }
    // Source - https://stackoverflow.com/a/35916241
    // Posted by Yerko Palma, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-02-06, License - CC BY-SA 4.0

    // from your component
    console.log("query parameter", this.$route.query.d) // outputs 'yay'
    this.device_settings.qrcode = this.$route.query.d

  },
  mounted() {

  },
  watch: {

  }
}
</script>
