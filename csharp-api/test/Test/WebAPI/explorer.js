const API_BASE = window.location.origin;

async function fetchJson(path) {
  const r = await fetch(API_BASE + path);
  return r.json();
}

function esc(s) {
  const d = document.createElement('div');
  d.textContent = s;
  return d.innerHTML;
}

// ── Tree node builder ─────────────────────────────────────────────

function createTreeNode(labelHtml, opts = {}) {
  const { expandable = true, onExpand, suffix } = opts;

  const node = document.createElement('div');
  node.className = 'tree-node';

  const header = document.createElement('div');
  header.className = 'tree-header';

  const arrow = document.createElement('span');
  arrow.className = expandable ? 'tree-arrow' : 'tree-arrow leaf';
  arrow.textContent = '\u25B6';
  header.appendChild(arrow);

  const label = document.createElement('span');
  label.innerHTML = labelHtml;
  header.appendChild(label);

  if (suffix) {
    const suf = document.createElement('span');
    suf.innerHTML = suffix;
    suf.style.marginLeft = '8px';
    header.appendChild(suf);
  }

  node.appendChild(header);

  const children = document.createElement('div');
  children.className = 'tree-children';
  node.appendChild(children);

  let loaded = false;
  if (expandable) {
    header.addEventListener('click', () => {
      const isOpen = children.classList.contains('open');
      if (isOpen) {
        children.classList.remove('open');
        arrow.classList.remove('expanded');
      } else {
        children.classList.add('open');
        arrow.classList.add('expanded');
        if (!loaded && onExpand) {
          loaded = true;
          const status = document.createElement('div');
          status.className = 'tree-status';
          status.textContent = 'Loading...';
          children.appendChild(status);
          onExpand(children).then(() => {
            children.removeChild(status);
          }).catch(e => {
            status.textContent = 'Error: ' + e.message;
            status.className = 'error-msg';
            status.style.paddingLeft = '20px';
          });
        }
      }
    });
  }

  return { node, children };
}

function createLeaf(html) {
  const leaf = document.createElement('div');
  leaf.className = 'tree-leaf';
  leaf.innerHTML = html;
  return leaf;
}

// ── Object loading ────────────────────────────────────────────────

function objParams(address, kind, typeName) {
  return `address=${encodeURIComponent(address)}&kind=${encodeURIComponent(kind)}&typeName=${encodeURIComponent(typeName)}`;
}

async function loadObjectInto(container, address, kind, typeName) {
  const data = await fetchJson(`/api/explorer/object?${objParams(address, kind, typeName)}`);
  if (data.error) {
    container.appendChild(createLeaf(`<span class="error-msg">${esc(data.error)}</span>`));
    return;
  }

  // Internals node
  const internals = createTreeNode('Internals', {
    onExpand: async (c) => {
      c.appendChild(createLeaf(`<span class="oe-type">${esc(data.typeName)}</span>`));
      c.appendChild(createLeaf(`Address: ${esc(data.address)}`));
      if (data.refCount != null) {
        c.appendChild(createLeaf(`Ref count: ${data.refCount}`));
      }
    }
  });
  container.appendChild(internals.node);

  // Fields node
  if (data.fields && data.fields.length > 0) {
    const fieldsNode = createTreeNode(`Fields <span style="color:#8b949e">(${data.fields.length})</span>`, {
      onExpand: async (c) => {
        for (const f of data.fields) {
          renderField(c, f, address, kind, typeName);
        }
      }
    });
    container.appendChild(fieldsNode.node);
  }

  // Methods node
  if (data.methods && data.methods.length > 0) {
    const methodsNode = createTreeNode(`Methods <span style="color:#8b949e">(${data.methods.length})</span>`, {
      onExpand: async (c) => {
        for (const m of data.methods) {
          renderMethod(c, m, address, kind, typeName);
        }
      }
    });
    container.appendChild(methodsNode.node);
  }

  // Array elements
  if (data.isArray && data.arrayLength != null && data.arrayLength > 0) {
    const arrayNode = createTreeNode(`Array <span style="color:#8b949e">(${data.arrayLength} elements)</span>`, {
      onExpand: async (c) => {
        await loadArrayElements(c, address, kind, typeName, 0, 50);
      }
    });
    container.appendChild(arrayNode.node);
  }
}

// ── Field rendering ───────────────────────────────────────────────

function renderField(container, f, parentAddress, parentKind, parentTypeName) {
  const typeLabel = `<span class="oe-type">${esc(f.typeName)}</span>`;
  const nameLabel = `<span class="oe-field">${esc(f.name)}</span>`;
  const staticLabel = f.isStatic ? ' <span class="oe-static">Static</span>' : '';
  const offsetLabel = !f.isStatic && f.offset ? ` <span style="color:#8b949e">${esc(f.offset)}</span>` : '';

  if (f.isValueType) {
    // Value type — show inline value, not expandable for deeper exploration
    const valueStr = f.value != null ? `<span class="oe-value">${esc(f.value)}</span>` : '<span class="oe-null">null</span>';
    const leaf = createTreeNode(`${typeLabel} ${nameLabel}${staticLabel}${offsetLabel}`, {
      expandable: true,
      onExpand: async (c) => {
        c.appendChild(createLeaf(`Value: ${valueStr}`));
      },
      suffix: valueStr
    });
    container.appendChild(leaf.node);
  } else {
    // Reference type — expandable to load child object
    const refNode = createTreeNode(`${typeLabel} ${nameLabel}${staticLabel}${offsetLabel}`, {
      onExpand: async (c) => {
        await loadFieldChild(c, parentAddress, parentKind, parentTypeName, f.name);
      }
    });
    container.appendChild(refNode.node);
  }
}

async function loadFieldChild(container, parentAddress, parentKind, parentTypeName, fieldName) {
  const data = await fetchJson(`/api/explorer/field?${objParams(parentAddress, parentKind, parentTypeName)}&fieldName=${encodeURIComponent(fieldName)}`);
  if (data.error) {
    container.appendChild(createLeaf(`<span class="error-msg">${esc(data.error)}</span>`));
    return;
  }
  if (data.isNull) {
    container.appendChild(createLeaf('<span class="oe-null">null</span>'));
    return;
  }
  await loadObjectInto(container, data.childAddress, data.childKind, data.childTypeName);
}

// ── Method rendering ──────────────────────────────────────────────

function renderMethod(container, m, parentAddress, parentKind, parentTypeName) {
  const retLabel = `<span class="oe-type">${esc(m.returnType)}</span>`;
  const nameLabel = `<span class="oe-method">${esc(m.name)}</span>`;
  const params = m.parameters.map(p => `<span class="oe-type">${esc(p.type)}</span> ${esc(p.name || '')}`).join(', ');

  if (m.isGetter) {
    const methodNode = createTreeNode(`${retLabel} ${nameLabel}(${params})`, {
      onExpand: async (c) => {
        await loadMethodResult(c, parentAddress, parentKind, parentTypeName, m.name, m.signature);
      }
    });
    container.appendChild(methodNode.node);
  } else {
    // Non-getter: show as non-expandable leaf-style node
    const { node } = createTreeNode(`${retLabel} ${nameLabel}(${params})`, { expandable: false });
    container.appendChild(node);
  }
}

async function loadMethodResult(container, parentAddress, parentKind, parentTypeName, methodName, methodSignature) {
  const sigParam = methodSignature ? `&methodSignature=${encodeURIComponent(methodSignature)}` : '';
  const data = await fetchJson(`/api/explorer/method?${objParams(parentAddress, parentKind, parentTypeName)}&methodName=${encodeURIComponent(methodName)}${sigParam}`);
  if (data.error) {
    container.appendChild(createLeaf(`<span class="error-msg">${esc(data.error)}</span>`));
    return;
  }
  if (data.isObject) {
    await loadObjectInto(container, data.childAddress, data.childKind, data.childTypeName);
  } else {
    container.appendChild(createLeaf(`Result: <span class="oe-value">${esc(data.value)}</span>`));
  }
}

// ── Array rendering ───────────────────────────────────────────────

async function loadArrayElements(container, address, kind, typeName, offset, count) {
  const data = await fetchJson(`/api/explorer/array?${objParams(address, kind, typeName)}&offset=${offset}&count=${count}`);
  if (data.error) {
    container.appendChild(createLeaf(`<span class="error-msg">${esc(data.error)}</span>`));
    return;
  }

  for (const el of data.elements) {
    if (el.isNull) {
      container.appendChild(createLeaf(`[${el.index}] <span class="oe-null">null</span>`));
    } else if (el.isObject) {
      const display = el.display ? ` <span class="oe-value">(${esc(el.display)})</span>` : '';
      const elNode = createTreeNode(`[${el.index}] <span class="oe-type">${esc(el.typeName)}</span>${display}`, {
        onExpand: async (c) => {
          await loadObjectInto(c, el.address, el.kind, el.typeName);
        }
      });
      container.appendChild(elNode.node);
    } else {
      container.appendChild(createLeaf(`[${el.index}] <span class="oe-value">${esc(el.value)}</span>`));
    }
  }

  if (data.hasMore) {
    const btn = document.createElement('button');
    btn.className = 'tree-load-more';
    btn.textContent = `Load more (${offset + data.count} / ${data.totalLength})`;
    btn.addEventListener('click', async () => {
      container.removeChild(btn);
      await loadArrayElements(container, address, kind, typeName, offset + data.count, count);
    });
    container.appendChild(btn);
  }
}

// ── Entry point ───────────────────────────────────────────────────

let managedEntries = [];
let nativeEntries = [];

async function loadExplorer() {
  const tree = document.getElementById('explorer-tree');
  tree.innerHTML = '<div class="tree-status">Loading singletons...</div>';

  try {
    const data = await fetchJson('/api/explorer/singletons');
    tree.innerHTML = '';

    managedEntries = data.managed || [];
    nativeEntries = data.native || [];

    renderSingletonGroups();
  } catch (e) {
    tree.innerHTML = `<span class="error-msg">Failed to load: ${esc(e.message)}</span>`;
  }
}

function renderSingletonGroups() {
  const tree = document.getElementById('explorer-tree');
  tree.innerHTML = '';
  const filter = document.getElementById('explorer-search').value.toLowerCase();

  const filteredManaged = managedEntries.filter(s => s.type.toLowerCase().includes(filter));
  const filteredNative = nativeEntries.filter(s => s.type.toLowerCase().includes(filter));

  // Managed Singletons group
  const managedGroup = createTreeNode(`<span class="tree-group-label">Managed Singletons (${filteredManaged.length})</span>`, {
    onExpand: async (c) => {
      for (const s of filteredManaged) {
        const sNode = createTreeNode(`<span class="oe-type">${esc(s.type)}</span>`, {
          onExpand: async (c2) => {
            await loadObjectInto(c2, s.address, s.kind, s.type);
          }
        });
        c.appendChild(sNode.node);
      }
    }
  });
  tree.appendChild(managedGroup.node);

  // Native Singletons group
  const nativeGroup = createTreeNode(`<span class="tree-group-label">Native Singletons (${filteredNative.length})</span>`, {
    onExpand: async (c) => {
      for (const s of filteredNative) {
        const sNode = createTreeNode(`<span class="oe-type">${esc(s.type)}</span>`, {
          onExpand: async (c2) => {
            await loadObjectInto(c2, s.address, s.kind, s.type);
          }
        });
        c.appendChild(sNode.node);
      }
    }
  });
  tree.appendChild(nativeGroup.node);
}

document.getElementById('explorer-search').addEventListener('input', renderSingletonGroups);

loadExplorer();
