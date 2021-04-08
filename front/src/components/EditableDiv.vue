<template>
  <div
    contenteditable="true"
    v-text="content"
    class="editable_text"
    :placeholder="placeholder"
    @input="handleInput"
    @compositionstart="handleCompositionStart"
    @compositionend="handleCompositionEnd"
    @keydown.enter.prevent="handleBlur"
    @keyup.esc="handleBlur"
  />
</template>

<script>
const DEFAULT_PLACEHOLDER = "--";

export default {
  model: {
    prop: "content",
    event: "change"
  },
  props: {
    content: {
      type: String,
      default: ""
    },
    placeholder: {
      type: String,
      default: DEFAULT_PLACEHOLDER
    }
  },
  data() {
    return {
      compositionStart: false
    };
  },
  methods: {
    handleInput(e) {
      if (!this.compositionStart) {
        this.changeContent(e);
      }
    },
    handleCompositionStart() {
      this.compositionStart = true;
    },
    handleCompositionEnd(e) {
      this.changeContent(e);
      this.compositionStart = false;
    },
    handleBlur(e) {
      e.target.blur();
    },
    changeContent(e) {
      let element = e.target;
      let index = getCaretPosition(element);
      setTimeout(() => {
        setCursor(element, index);
      }, 0);
      this.$emit("change", element.textContent);
    }
  }
};

function getCaretPosition(editableDiv) {
  var caretPos = 0,
    sel,
    range;
  if (window.getSelection) {
    sel = window.getSelection();
    if (sel.rangeCount) {
      range = sel.getRangeAt(0);
      if (range.commonAncestorContainer.parentNode == editableDiv) {
        caretPos = range.endOffset;
      }
    }
  } else if (document.selection && document.selection.createRange) {
    range = document.selection.createRange();
    if (range.parentElement() == editableDiv) {
      var tempEl = document.createElement("span");
      editableDiv.insertBefore(tempEl, editableDiv.firstChild);
      var tempRange = range.duplicate();
      tempRange.moveToElementText(tempEl);
      tempRange.setEndPoint("EndToEnd", range);
      caretPos = tempRange.text.length;
    }
  }
  return caretPos;
}

function setCursor(el, index) {
  if (el.textContent && el.textContent.length > 0) {
    const range = document.createRange();
    const sel = window.getSelection();
    range.setStart(el.childNodes[0], index);
    range.collapse(true);
    sel.removeAllRanges();
    sel.addRange(range);
  }
}
</script>

<style scoped>
[contenteditable="true"]:empty:before {
  content: attr(placeholder);
  color: gray;
}
[contenteditable="true"]:focus:before {
  content: none;
}
</style>