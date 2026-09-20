<template>
  <v-container>
    <v-toolbar>
      <v-app-bar-nav-icon @click="user_save_user_info(user_info)" :disabled="!isFormValid">
        <v-icon>mdi-arrow-left-circle-outline</v-icon>
      </v-app-bar-nav-icon>

      <v-toolbar-title>{{ $t("main_title") }} &#x2022; {{ $t("profile_header") }} {{ user_info.username
      }}</v-toolbar-title>
    </v-toolbar>
    <!--v-container-->
    <v-card class="mx-auto">
      <v-form ref="form" v-model="isFormValid">
        <v-text-field minLength=5 maxLength=80 v-model="user_info.nickname" :label="$t('profile_name_help')" required
          :rules="[rules.required, rules.counter]"></v-text-field>
        <v-text-field maxLength=100 v-model="user_info.info" :label="$t('profile_info_help')"></v-text-field>
        <v-textarea maxLength=600 v-model="user_info.description" :label="$t('profile_description_help')"></v-textarea>
        <v-select :label="$t('profile_newsletter')" v-model="user_info.newsletter" :items="newsletter_items"
          item-text="title" item-value="index">
        </v-select>
        <v-checkbox v-model="user_info.friendsoffriends" v-bind:id="user_info.uuid"
          :label="$t('profile_friendoffriends_button')">{{ $t('profile_friendoffriends_button') }}</v-checkbox>
        <v-checkbox v-model="user_info.public" v-bind:id="user_info.uuid" :label="$t('profile_public_help')">{{
          $t('profile_public_help') }}</v-checkbox>
        <!--
        <v-btn text @click="user_save_user_info(user_info)" :disabled="!isFormValid">
          {{ $t('edit_save') }}
        </v-btn>
        -->
        <v-divider></v-divider>
        <v-btn text @click="user_user_logout()">
          {{ $t('profile_logout') }}
        </v-btn>
        <v-divider></v-divider>
        <v-btn @click="profile_delete_dialog_show = true" color=red>
          <v-icon>mdi-delete</v-icon>
          {{ $t("profile_delete_account") }}
        </v-btn>
      </v-form>
      <v-divider></v-divider>
      <div v-if="user_info.foflist.length > 0">
        <div :class="['text-caption', 'text-decoration-underline', 'pa-2']">{{ $t("profile_foflist_header") }}</div>
        <v-list-item v-for="friend in user_info.foflist" v-bind:key="friend.value">
          <v-list-item-content>
            <v-list-item-title>{{ friend.title }}</v-list-item-title>
          </v-list-item-content>
        </v-list-item>
      </div>
      <!--/v-container-->
      <v-dialog v-model="profile_delete_dialog_show" scrollable max-width="300px">
        <v-card>
          <v-card-title>{{ $t("profile_delete_account") }}</v-card-title>
          <v-card-text>{{ $t("profile_delete_account_confirm") }}</v-card-text>
          <v-divider></v-divider>
          <v-card-actions>
            <v-btn color="blue darken-1" text @click="profile_delete_dialog_show = false">{{
              $t("cardmenu_delete_dialog_cancel") }}</v-btn>
            <v-btn color="red darken-1" text @click="user_user_delete()">{{ $t("profile_delete_account_select")
            }}</v-btn>
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
  name: 'Devices',
  data() {
    return {
      isFormValid: false,
      settings_delete_dialog_show: false,
      newsletter_items: [
        { index: 0, title: this.$t('profile_newsletter_none') },
        { index: 1, title: this.$t('profile_newsletter_relevant') },
        { index: 2, title: this.$t('profile_newsletter_all') }
      ],
      rules: {
        required: value => !!value || this.$t('edit_min_5_chars'),
        counter: value => value.length >= 5 || this.$t('profile_min_chars'),
      }
    }
  },
  methods: {
    user_user_logout() {
      messenger.emit("user_user_logout", {
      });
      this.nav2Settings()
    },
    user_user_delete() {
      messenger.emit("user_user_delete", {
      });
      this.nav2Settings()
    },
    user_save_user_info() {
      messenger.emit("user_save_user_info", {
        user_info: this.user_info
      });
      this.nav2Settings()
    },
    nav2Settings() {
      router.push({ name: "Settings" })
    },
  },
  created() {
    console.log("user_info", this.user_info)
  },
  mounted() {

  },
  watch: {

  }
}
</script>
