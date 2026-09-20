<template>
  <v-container>
    <h1>{{ $t('login') }}</h1>
    {{ $t('login_name_intro') }}
    <v-form ref="form" v-model="isFormValid">
      <v-text-field v-model="userName" :label="$t('login_name_help')"
        :rules="[rules.required, rules.email]"></v-text-field>
      <v-text-field v-model="pw" type="password" :label="$t('login_password_help')"
        :rules="[rules.required, rules.counter]"></v-text-field>
      <v-text-field v-model="pw_confirm" type="password" :label="$t('login_confirm_password_help')"
        :rules="[rules.required, rules.counter, rules.equal]" v-if="request_new_password"></v-text-field>
      <v-btn text @click="login_request(false)" :disabled="!isFormValid" v-if="!request_new_password">
        {{ $t('login_button_text') }}
      </v-btn>
      <div :class="[`text-body-1`, `mb-4`]" v-text="$t(errormessage)">
      </div>
      <v-btn color="pink" text @click="login_request(true)" :disabled="!isFormValid" v-if="request_new_password">
        {{ $t('login_password_forgot') }}
      </v-btn>
      <br v-if="request_new_password" />
      <v-btn color="pink" text @click="request_new_password = !request_new_password">
        {{ !request_new_password ? $t('login_password_request') : $t('back') }}
      </v-btn>
    </v-form>
  </v-container>
</template>

<script>
import messenger from "../messenger";
import router from "../router";
export default {
  name: 'Login',
  data() {
    return {
      userName: '',
      pw: '',
      pw_confirm: '',
      token: '',
      errormessage: '',
      isFormValid: false,
      request_new_password: false,
      rules: {
        required: value => !!value || this.$t('edit_min_5_chars'),
        counter: value => value.length >= 8 || this.$t('login_min_chars'),
        equal: value => value == this.pw || this.$t('login_passwords_not_equal'),
        email: value => {
          const pattern = /^(([^<>()[\]\\.,;:\s@"]+(\.[^<>()[\]\\.,;:\s@"]+)*)|(".+"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$/
          return pattern.test(value) || this.$t('login_invalid_email')
        },
      },

    }
  },
  created() {
    messenger.register(
      "login",
      this.messenger_onMessage,
      null,
      null
    );

  },
  methods: {
    messenger_onMessage(type, data) {
      console.log("incoming message to login", type, data);
      if (type == "login_login") {
        if (data.errormessage) {
          this.errormessage = data.errormessage
        } else {
          this.errormessage = ''
          localStorage.token = data.token
          if (data.token && data.token != "") {
            this.nav2Home()
          }
        }
      }
    },
    login_request(request_pw) {

      messenger.emit("login_request", {
        name: localStorage.userName,
        pw: this.pw,
        request_pw: request_pw
      });
    },
    nav2Home() {
      router.push({ name: "Home" });
    },
    nav2Login() {
      router.push({ name: "Login" });
    },
  },
  mounted() {

  },
  watch: {
    userName(newUser) {
      localStorage.userName = newUser
    },
    token(newToken) {
      localStorage.token = newToken
    },
  }
}
</script>
