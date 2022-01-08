<template>
  <div>
    <main>
      <OutputState title="Hlavní světla"/>

      <OutputState title="Čerpadlo" :active="true"/>
    </main>
  </div>
</template>

<script setup lang="ts">
import OutputState from "~/components/OutputState.vue"
</script>
