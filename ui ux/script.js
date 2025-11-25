// ===== State Management =====
let state = {
    network: null,
    dataset: null,
    isTraining: false,
    trainingInterval: null,
    currentEpoch: 0,
    maxEpochs: 100,
    errorData: [],
    theme: 'light',
    stepMode: false,
    trainingHistory: [],
    startTime: null,
    confusionMatrix: null,
    metrics: {
        accuracy: null,
        mse: null,
        minError: null,
        trainingTime: 0
    }
};

// ===== Theme Management =====
function toggleTheme() {
    const body = document.body;
    const themeIcon = document.getElementById('themeIcon');
    
    if (body.classList.contains('light-theme')) {
        body.classList.remove('light-theme');
        body.classList.add('dark-theme');
        themeIcon.className = 'fas fa-sun';
        state.theme = 'dark';
    } else {
        body.classList.remove('dark-theme');
        body.classList.add('light-theme');
        themeIcon.className = 'fas fa-moon';
        state.theme = 'light';
    }
}

// ===== Modal Management =====
function openModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.classList.add('active');
    }
}

function closeModal(modalId) {
    const modal = document.getElementById(modalId);
    if (modal) {
        modal.classList.remove('active');
    }
}

function openNetworkDialog() {
    openModal('networkDialog');
}

function closeNetworkDialog() {
    closeModal('networkDialog');
}

function openDatasetDialog() {
    openModal('datasetDialog');
}

function closeDatasetDialog() {
    closeModal('datasetDialog');
}

function openTrainingDialog() {
    openModal('trainingDialog');
}

function closeTrainingDialog() {
    closeModal('trainingDialog');
}

function showAbout() {
    openModal('aboutDialog');
}

function closeAbout() {
    closeModal('aboutDialog');
}

// Close modal on outside click
document.addEventListener('click', (e) => {
    if (e.target.classList.contains('modal')) {
        e.target.classList.remove('active');
    }
});

// ===== Network Management =====
function addLayer() {
    const layerList = document.getElementById('layerList');
    const layerCount = layerList.children.length;
    
    const layerItem = document.createElement('div');
    layerItem.className = 'layer-item';
    layerItem.innerHTML = `
        <span class="layer-label">Couche ${layerCount + 1}</span>
        <input type="number" class="layer-input" value="4" min="1" placeholder="Neurones">
        <select class="layer-activation">
            <option>Sigmoid</option>
            <option>ReLU</option>
            <option>Tanh</option>
            <option>Linéaire</option>
        </select>
    `;
    
    layerList.appendChild(layerItem);
}

function removeLayer() {
    const layerList = document.getElementById('layerList');
    if (layerList.children.length > 1) {
        layerList.removeChild(layerList.lastChild);
    }
}

function createNetwork() {
    const layerList = document.getElementById('layerList');
    const layers = [];
    
    Array.from(layerList.children).forEach((layer, index) => {
        const input = layer.querySelector('.layer-input');
        const activation = layer.querySelector('.layer-activation');
        layers.push({
            neurons: parseInt(input.value) || 2,
            activation: activation.value,
            isInput: index === 0
        });
    });
    
    state.network = {
        layers: layers,
        created: new Date().toLocaleString()
    };
    
    updateNetworkInfo();
    drawNetwork();
    closeNetworkDialog();
    addLog('Réseau créé avec succès', 'success');
    updateStatus();
}

function updateNetworkInfo() {
    const networkInfo = document.getElementById('networkInfo');
    if (state.network) {
        const layersInfo = state.network.layers.map((l, i) => 
            `Couche ${i + 1}: ${l.neurons} neurones (${l.activation})`
        ).join('<br>');
        
        networkInfo.innerHTML = `
            <p><strong>Architecture:</strong></p>
            <p>${layersInfo}</p>
            <p style="margin-top: 8px; font-size: 11px; color: var(--text-secondary);">
                Créé: ${state.network.created}
            </p>
        `;
        
        document.getElementById('statusNetwork').textContent = 
            `Réseau: ${state.network.layers.length} couches`;
    } else {
        networkInfo.innerHTML = '<p class="info-placeholder">Aucun réseau créé</p>';
    }
}

// ===== Network Visualization =====
function drawNetwork() {
    const canvas = document.getElementById('networkCanvas');
    const placeholder = document.getElementById('networkPlaceholder');
    
    if (!state.network) {
        canvas.style.display = 'none';
        placeholder.style.display = 'block';
        return;
    }
    
    canvas.style.display = 'block';
    placeholder.style.display = 'none';
    
    const ctx = canvas.getContext('2d');
    const rect = canvas.getBoundingClientRect();
    canvas.width = rect.width;
    canvas.height = rect.height;
    
    const layers = state.network.layers;
    const layerSpacing = canvas.width / (layers.length + 1);
    const maxNeurons = Math.max(...layers.map(l => l.neurons));
    const nodeRadius = 20;
    const nodeSpacing = canvas.height / (maxNeurons + 1);
    
    // Clear canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    // Draw connections with gradient
    ctx.lineWidth = 1.5;
    
    for (let i = 0; i < layers.length - 1; i++) {
        const currentLayer = layers[i];
        const nextLayer = layers[i + 1];
        const currentX = layerSpacing * (i + 1);
        const nextX = layerSpacing * (i + 2);
        
        for (let j = 0; j < currentLayer.neurons; j++) {
            const currentY = nodeSpacing * (j + 1) + (canvas.height - nodeSpacing * currentLayer.neurons) / 2;
            
            for (let k = 0; k < nextLayer.neurons; k++) {
                const nextY = nodeSpacing * (k + 1) + (canvas.height - nodeSpacing * nextLayer.neurons) / 2;
                
                // Create gradient for connection
                const gradient = ctx.createLinearGradient(currentX + nodeRadius, currentY, nextX - nodeRadius, nextY);
                gradient.addColorStop(0, 'rgba(59, 130, 246, 0.3)');
                gradient.addColorStop(0.5, 'rgba(139, 92, 246, 0.4)');
                gradient.addColorStop(1, 'rgba(16, 185, 129, 0.3)');
                
                ctx.beginPath();
                ctx.moveTo(currentX + nodeRadius, currentY);
                ctx.lineTo(nextX - nodeRadius, nextY);
                ctx.strokeStyle = gradient;
                ctx.stroke();
            }
        }
    }
    
    // Draw nodes
    layers.forEach((layer, layerIndex) => {
        const x = layerSpacing * (layerIndex + 1);
        const nodeSpacingForLayer = canvas.height / (layer.neurons + 1);
        
        for (let i = 0; i < layer.neurons; i++) {
            const y = nodeSpacingForLayer * (i + 1);
            
            // Node circle with gradient
            const gradient = ctx.createRadialGradient(x - nodeRadius/3, y - nodeRadius/3, 0, x, y, nodeRadius);
            if (layer.isInput) {
                gradient.addColorStop(0, '#60a5fa');
                gradient.addColorStop(1, '#3b82f6');
            } else if (layerIndex === layers.length - 1) {
                gradient.addColorStop(0, '#34d399');
                gradient.addColorStop(1, '#10b981');
            } else {
                gradient.addColorStop(0, '#a78bfa');
                gradient.addColorStop(1, '#8b5cf6');
            }
            
            ctx.beginPath();
            ctx.arc(x, y, nodeRadius, 0, 2 * Math.PI);
            ctx.fillStyle = gradient;
            ctx.fill();
            ctx.strokeStyle = 'rgba(255, 255, 255, 0.8)';
            ctx.lineWidth = 2.5;
            ctx.stroke();
            
            // Glow effect
            ctx.shadowBlur = 10;
            ctx.shadowColor = layer.isInput ? '#3b82f6' : (layerIndex === layers.length - 1 ? '#10b981' : '#8b5cf6');
            ctx.stroke();
            ctx.shadowBlur = 0;
            
            // Node label
            ctx.fillStyle = 'white';
            ctx.font = '12px Arial';
            ctx.textAlign = 'center';
            ctx.textBaseline = 'middle';
            ctx.fillText((i + 1).toString(), x, y);
        }
        
        // Layer label
        ctx.fillStyle = getComputedStyle(document.documentElement).getPropertyValue('--text-primary').trim() || '#1a1a1a';
        ctx.font = 'bold 14px Arial';
        ctx.textAlign = 'center';
        ctx.fillText(
            layer.isInput ? 'Entrée' : `Couche ${layerIndex}`,
            x,
            canvas.height - 20
        );
    });
}

// ===== Dataset Management =====
function browseFile() {
    // Simulate file browser
    const path = document.getElementById('datasetPath');
    path.value = 'data/XOR_dataset.csv';
    addLog('Fichier sélectionné: XOR_dataset.csv', 'info');
}

function loadDataset() {
    const path = document.getElementById('datasetPath').value;
    const numInputs = parseInt(document.getElementById('numInputs').value) || 2;
    const numOutputs = parseInt(document.getElementById('numOutputs').value) || 1;
    const hasHeader = document.getElementById('hasHeader').checked;
    
    if (!path) {
        showNotification('Veuillez sélectionner un fichier', true);
        return;
    }
    
    state.dataset = {
        path: path,
        inputs: numInputs,
        outputs: numOutputs,
        hasHeader: hasHeader,
        loaded: new Date().toLocaleString()
    };
    
    updateDatasetInfo();
    closeDatasetDialog();
    addLog(`Dataset chargé: ${path}`, 'success');
    updateStatus();
}

function updateDatasetInfo() {
    const datasetInfo = document.getElementById('datasetInfo');
    if (state.dataset) {
        datasetInfo.innerHTML = `
            <p><strong>Fichier:</strong> ${state.dataset.path.split('/').pop()}</p>
            <p><strong>Entrées:</strong> ${state.dataset.inputs}</p>
            <p><strong>Sorties:</strong> ${state.dataset.outputs}</p>
            <p style="margin-top: 8px; font-size: 11px; color: var(--text-secondary);">
                Chargé: ${state.dataset.loaded}
            </p>
        `;
        
        document.getElementById('statusDataset').textContent = 
            `Dataset: ${state.dataset.path.split('/').pop()}`;
    } else {
        datasetInfo.innerHTML = '<p class="info-placeholder">Aucun dataset chargé</p>';
    }
}

// ===== Training Management =====
function saveTrainingParams() {
    const learningRate = parseFloat(document.getElementById('learningRate').value) || 0.1;
    const numEpochs = parseInt(document.getElementById('numEpochs').value) || 100;
    const batchSize = parseInt(document.getElementById('batchSize').value) || 32;
    const momentum = parseFloat(document.getElementById('momentum').value) || 0.9;
    const shuffle = document.getElementById('shuffleData').checked;
    
    state.maxEpochs = numEpochs;
    
    addLog(`Paramètres d'entraînement enregistrés: LR=${learningRate}, Epochs=${numEpochs}`, 'info');
    closeTrainingDialog();
}

function startTraining() {
    if (!state.network) {
        showNotification('Veuillez créer un réseau d\'abord', true);
        return;
    }
    
    if (!state.dataset) {
        showNotification('Veuillez charger un dataset d\'abord', true);
        return;
    }
    
    if (state.isTraining) {
        return;
    }
    
    state.isTraining = true;
    state.currentEpoch = 0;
    state.errorData = [];
    
    document.getElementById('btnStartTraining').disabled = true;
    const stopBtn = document.getElementById('btnStopTraining');
    stopBtn.disabled = false;
    stopBtn.classList.add('stop');
    document.getElementById('progressContainer').style.display = 'block';
    
    updateTrainingInfo();
    updateStatus();
    addLog('Entraînement démarré', 'success');
    
    // Simulate training
    state.trainingInterval = setInterval(() => {
        state.currentEpoch++;
        const error = Math.max(0.01, 1 - (state.currentEpoch / state.maxEpochs) + Math.random() * 0.1);
        state.errorData.push({ epoch: state.currentEpoch, error: error });
        
        updateProgress();
        updateErrorChart();
        updateTrainingInfo();
        
        if (state.currentEpoch >= state.maxEpochs) {
            stopTraining();
            addLog('Entraînement terminé avec succès', 'success');
        }
    }, 100);
}

function stopTraining() {
    if (!state.isTraining) {
        return;
    }
    
    state.isTraining = false;
    
    if (state.trainingInterval) {
        clearInterval(state.trainingInterval);
        state.trainingInterval = null;
    }
    
    document.getElementById('btnStartTraining').disabled = false;
    const stopBtn = document.getElementById('btnStopTraining');
    stopBtn.disabled = true;
    stopBtn.classList.remove('stop');
    
    updateStatus();
    addLog('Entraînement arrêté', 'warning');
}

function updateProgress() {
    const percent = Math.min(100, (state.currentEpoch / state.maxEpochs) * 100);
    document.getElementById('progressFill').style.width = percent + '%';
    document.getElementById('progressPercent').textContent = Math.round(percent) + '%';
    document.getElementById('progressText').textContent = 
        `Époque ${state.currentEpoch} / ${state.maxEpochs}`;
}

function updateTrainingInfo() {
    const trainingInfo = document.getElementById('trainingInfo');
    if (state.isTraining) {
        const currentError = state.errorData.length > 0 ? 
            state.errorData[state.errorData.length - 1].error.toFixed(4) : '-';
        const minError = state.errorData.length > 0 ?
            Math.min(...state.errorData.map(d => d.error)).toFixed(4) : '-';
        
        trainingInfo.innerHTML = `
            <p><strong>Statut:</strong> En cours</p>
            <p><strong>Époque:</strong> ${state.currentEpoch} / ${state.maxEpochs}</p>
            <p><strong>Erreur actuelle:</strong> ${currentError}</p>
            <p><strong>Erreur minimale:</strong> ${minError}</p>
        `;
    } else {
        trainingInfo.innerHTML = '<p class="info-placeholder">Prêt à démarrer</p>';
    }
}

function updateStatus() {
    const statusText = document.getElementById('statusText');
    const statusIcon = statusText.previousElementSibling;
    
    if (state.isTraining) {
        statusText.textContent = 'Entraînement en cours...';
        statusIcon.className = 'fas fa-circle status-training';
    } else if (state.network && state.dataset) {
        statusText.textContent = 'Prêt à entraîner';
        statusIcon.className = 'fas fa-circle status-ready';
    } else {
        statusText.textContent = 'Prêt';
        statusIcon.className = 'fas fa-circle status-ready';
    }
}

// ===== Error Chart =====
function updateErrorChart() {
    const canvas = document.getElementById('errorChart');
    const placeholder = document.getElementById('chartPlaceholder');
    
    if (state.errorData.length === 0) {
        canvas.style.display = 'none';
        placeholder.style.display = 'block';
        return;
    }
    
    canvas.style.display = 'block';
    placeholder.style.display = 'none';
    
    const ctx = canvas.getContext('2d');
    const rect = canvas.getBoundingClientRect();
    canvas.width = rect.width;
    canvas.height = rect.height;
    
    const margin = { top: 20, right: 20, bottom: 40, left: 60 };
    const width = canvas.width - margin.left - margin.right;
    const height = canvas.height - margin.top - margin.bottom;
    
    // Clear
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    
    if (state.errorData.length === 0) return;
    
    // Find min/max
    const errors = state.errorData.map(d => d.error);
    const epochs = state.errorData.map(d => d.epoch);
    const minError = Math.min(...errors);
    const maxError = Math.max(...errors);
    const maxEpoch = Math.max(...epochs);
    
    // Draw axes
    ctx.strokeStyle = getComputedStyle(document.documentElement).getPropertyValue('--text-secondary').trim() || '#6b7280';
    ctx.lineWidth = 1;
    
    // X axis
    ctx.beginPath();
    ctx.moveTo(margin.left, canvas.height - margin.bottom);
    ctx.lineTo(canvas.width - margin.right, canvas.height - margin.bottom);
    ctx.stroke();
    
    // Y axis
    ctx.beginPath();
    ctx.moveTo(margin.left, margin.top);
    ctx.lineTo(margin.left, canvas.height - margin.bottom);
    ctx.stroke();
    
    // Draw grid with subtle colors
    for (let i = 0; i <= 5; i++) {
        const y = margin.top + (height / 5) * i;
        ctx.strokeStyle = i === 5 ? 'rgba(59, 130, 246, 0.3)' : 'rgba(107, 114, 128, 0.15)';
        ctx.lineWidth = i === 5 ? 1.5 : 1;
        ctx.setLineDash(i === 5 ? [] : [5, 5]);
        ctx.beginPath();
        ctx.moveTo(margin.left, y);
        ctx.lineTo(canvas.width - margin.right, y);
        ctx.stroke();
        ctx.setLineDash([]);
    }
    
    // Draw line with gradient
    const gradient = ctx.createLinearGradient(margin.left, margin.top, margin.left, margin.top + height);
    gradient.addColorStop(0, '#3b82f6');
    gradient.addColorStop(0.5, '#8b5cf6');
    gradient.addColorStop(1, '#10b981');
    
    ctx.strokeStyle = gradient;
    ctx.lineWidth = 3;
    ctx.lineCap = 'round';
    ctx.lineJoin = 'round';
    ctx.beginPath();
    
    state.errorData.forEach((point, index) => {
        const x = margin.left + (point.epoch / maxEpoch) * width;
        const y = margin.top + height - ((point.error - minError) / (maxError - minError)) * height;
        
        if (index === 0) {
            ctx.moveTo(x, y);
        } else {
            ctx.lineTo(x, y);
        }
    });
    
    ctx.stroke();
    
    // Draw points with glow
    state.errorData.forEach((point, index) => {
        const x = margin.left + (point.epoch / maxEpoch) * width;
        const y = margin.top + height - ((point.error - minError) / (maxError - minError)) * height;
        
        // Glow effect
        ctx.shadowBlur = 8;
        ctx.shadowColor = index === state.errorData.length - 1 ? '#10b981' : '#3b82f6';
        
        // Point gradient
        const pointGradient = ctx.createRadialGradient(x, y, 0, x, y, 5);
        pointGradient.addColorStop(0, index === state.errorData.length - 1 ? '#34d399' : '#60a5fa');
        pointGradient.addColorStop(1, index === state.errorData.length - 1 ? '#10b981' : '#3b82f6');
        
        ctx.beginPath();
        ctx.arc(x, y, 5, 0, 2 * Math.PI);
        ctx.fillStyle = pointGradient;
        ctx.fill();
        ctx.strokeStyle = 'white';
        ctx.lineWidth = 1.5;
        ctx.stroke();
        
        ctx.shadowBlur = 0;
    });
    
    // Labels
    ctx.fillStyle = getComputedStyle(document.documentElement).getPropertyValue('--text-primary').trim() || '#1a1a1a';
    ctx.font = '12px Arial';
    ctx.textAlign = 'center';
    ctx.fillText('Époque', canvas.width / 2, canvas.height - 10);
    
    ctx.save();
    ctx.translate(15, canvas.height / 2);
    ctx.rotate(-Math.PI / 2);
    ctx.fillText('Erreur', 0, 0);
    ctx.restore();
    
    // Update stats
    if (state.errorData.length > 0) {
        const current = state.errorData[state.errorData.length - 1];
        document.getElementById('currentEpoch').textContent = current.epoch;
        document.getElementById('currentError').textContent = current.error.toFixed(4);
        document.getElementById('minError').textContent = minError.toFixed(4);
    }
}

// ===== Tab Management =====
function switchTab(tabName) {
    document.querySelectorAll('.tab-btn').forEach(btn => btn.classList.remove('active'));
    document.querySelectorAll('.tab-content').forEach(content => content.classList.remove('active'));
    
    if (tabName === 'chart') {
        document.querySelector('.tab-btn:first-child').classList.add('active');
        document.getElementById('chartTab').classList.add('active');
    } else {
        document.querySelector('.tab-btn:last-child').classList.add('active');
        document.getElementById('logsTab').classList.add('active');
    }
}

// ===== Logs Management =====
function addLog(message, type = 'info') {
    const logsContainer = document.getElementById('logsContainer');
    const time = new Date().toLocaleTimeString();
    
    const logEntry = document.createElement('div');
    logEntry.className = `log-entry ${type}`;
    logEntry.innerHTML = `
        <span class="log-time">[${time}]</span>
        <span class="log-message">${message}</span>
    `;
    
    logsContainer.appendChild(logEntry);
    logsContainer.scrollTop = logsContainer.scrollHeight;
}

function clearLogs() {
    const logsContainer = document.getElementById('logsContainer');
    logsContainer.innerHTML = '';
    addLog('Journal effacé', 'info');
}

// ===== Utility Functions =====
function showNotification(message, isError = false) {
    addLog(message, isError ? 'error' : 'info');
    // You could add a toast notification here
}

function testNetwork() {
    if (!state.network) {
        showNotification('Veuillez créer un réseau d\'abord', true);
        return;
    }
    
    addLog('Test du réseau en cours...', 'info');
    setTimeout(() => {
        addLog('Test terminé: Précision = 95.5%', 'success');
    }, 1000);
}

function showWizard() {
    showNotification('Assistant de configuration', 'info');
    // You could implement a step-by-step wizard here
}

function zoomIn() {
    addLog('Zoom avant', 'info');
}

function zoomOut() {
    addLog('Zoom arrière', 'info');
}

function resetZoom() {
    addLog('Zoom réinitialisé', 'info');
    drawNetwork();
}

// ===== Initialization =====
window.addEventListener('resize', () => {
    drawNetwork();
    updateErrorChart();
});

// ===== View Navigation =====
function switchView(viewName) {
    document.querySelectorAll('.nav-tab').forEach(tab => tab.classList.remove('active'));
    document.querySelectorAll('.view-content').forEach(view => view.classList.remove('active'));
    
    event.target.classList.add('active');
    document.getElementById(viewName + 'View').classList.add('active');
    
    if (viewName === 'network') {
        drawNetwork();
    } else if (viewName === 'training') {
        updateErrorChart();
    } else if (viewName === 'metrics') {
        updateMetrics();
        drawConfusionMatrix();
    } else if (viewName === 'dataset') {
        updateDatasetView();
    } else if (viewName === 'dashboard') {
        updateDashboard();
    }
}

// ===== Dashboard Updates =====
function updateDashboard() {
    updateStatCards();
    updateDashboardNetworkInfo();
    updateActivityList();
}

function updateStatCards() {
    document.getElementById('statNetwork').textContent = 
        state.network ? `${state.network.layers.length} couches` : 'Non créé';
    document.getElementById('statDataset').textContent = 
        state.dataset ? state.dataset.path.split('/').pop() : 'Non chargé';
    
    if (state.errorData.length > 0) {
        const currentError = state.errorData[state.errorData.length - 1].error;
        document.getElementById('statError').textContent = currentError.toFixed(4);
    } else {
        document.getElementById('statError').textContent = '-';
    }
    
    document.getElementById('statEpoch').textContent = 
        `${state.currentEpoch}/${state.maxEpochs}`;
}

function updateDashboardNetworkInfo() {
    const info = document.getElementById('dashboardNetworkInfo');
    if (state.network) {
        const layersInfo = state.network.layers.map((l, i) => 
            `Couche ${i + 1}: ${l.neurons} neurones`
        ).join('<br>');
        info.innerHTML = `
            <p><strong>Architecture:</strong></p>
            <p>${layersInfo}</p>
            <p style="margin-top: 10px;"><strong>Total neurones:</strong> ${
                state.network.layers.reduce((sum, l) => sum + l.neurons, 0)
            }</p>
        `;
    } else {
        info.innerHTML = '<p class="info-placeholder">Créez un réseau pour voir les détails</p>';
    }
}

function updateActivityList() {
    const list = document.getElementById('activityList');
    // Keep only last 5 activities
    const activities = Array.from(list.children).slice(0, 5);
    list.innerHTML = '';
    activities.forEach(activity => list.appendChild(activity));
}

// ===== Network Templates =====
function showNetworkTemplates() {
    openNetworkDialog();
}

function applyTemplate(templateType) {
    const layerList = document.getElementById('layerList');
    layerList.innerHTML = '';
    
    let layers = [];
    switch(templateType) {
        case 'simple':
            layers = [
                { neurons: 2, label: 'Couche 1 (Entrée)' },
                { neurons: 4, label: 'Couche 2' },
                { neurons: 1, label: 'Couche 3 (Sortie)' }
            ];
            break;
        case 'medium':
            layers = [
                { neurons: 2, label: 'Couche 1 (Entrée)' },
                { neurons: 8, label: 'Couche 2' },
                { neurons: 4, label: 'Couche 3' },
                { neurons: 1, label: 'Couche 4 (Sortie)' }
            ];
            break;
        case 'deep':
            layers = [
                { neurons: 4, label: 'Couche 1 (Entrée)' },
                { neurons: 16, label: 'Couche 2' },
                { neurons: 8, label: 'Couche 3' },
                { neurons: 4, label: 'Couche 4' },
                { neurons: 1, label: 'Couche 5 (Sortie)' }
            ];
            break;
    }
    
    layers.forEach((layer, index) => {
        const layerItem = document.createElement('div');
        layerItem.className = 'layer-item';
        layerItem.innerHTML = `
            <span class="layer-label">${layer.label}</span>
            <input type="number" class="layer-input" value="${layer.neurons}" min="1" placeholder="Neurones">
            <select class="layer-activation">
                ${index === 0 ? '<option>Linéaire</option>' : ''}
                ${index > 0 ? '<option>Sigmoid</option><option>ReLU</option><option>Tanh</option><option>Linéaire</option>' : ''}
            </select>
        `;
        layerList.appendChild(layerItem);
    });
    
    addLog(`Template "${templateType}" appliqué`, 'success');
}

// ===== Step Mode =====
function toggleStepMode() {
    state.stepMode = !state.stepMode;
    const btn = document.getElementById('stepModeBtn');
    if (state.stepMode) {
        btn.classList.add('active');
        addLog('Mode pas à pas activé', 'info');
    } else {
        btn.classList.remove('active');
        addLog('Mode pas à pas désactivé', 'info');
    }
}

// ===== Metrics =====
function updateMetrics() {
    if (state.errorData.length > 0) {
        const minError = Math.min(...state.errorData.map(d => d.error));
        const currentError = state.errorData[state.errorData.length - 1].error;
        
        document.getElementById('metricMSE').textContent = currentError.toFixed(6);
        document.getElementById('metricMinError').textContent = minError.toFixed(6);
        document.getElementById('metricAccuracy').textContent = 
            state.metrics.accuracy ? (state.metrics.accuracy * 100).toFixed(2) + '%' : '-';
        document.getElementById('metricTime').textContent = 
            formatTime(state.metrics.trainingTime);
    }
}

function formatTime(seconds) {
    if (seconds < 60) return seconds + 's';
    const mins = Math.floor(seconds / 60);
    const secs = seconds % 60;
    return mins + 'm ' + secs + 's';
}

// ===== Confusion Matrix =====
function drawConfusionMatrix() {
    const canvas = document.getElementById('confusionMatrix');
    const placeholder = document.getElementById('confusionPlaceholder');
    
    if (!state.confusionMatrix) {
        canvas.style.display = 'none';
        placeholder.style.display = 'block';
        return;
    }
    
    canvas.style.display = 'block';
    placeholder.style.display = 'none';
    
    const ctx = canvas.getContext('2d');
    const rect = canvas.getBoundingClientRect();
    canvas.width = rect.width;
    canvas.height = rect.height;
    
    const matrix = state.confusionMatrix;
    const size = matrix.length;
    const cellWidth = canvas.width / size;
    const cellHeight = canvas.height / size;
    
    // Find max value for normalization
    const maxVal = Math.max(...matrix.flat());
    
    // Draw cells
    matrix.forEach((row, i) => {
        row.forEach((val, j) => {
            const x = j * cellWidth;
            const y = i * cellHeight;
            const intensity = val / maxVal;
            
            // Color gradient
            const r = Math.floor(59 + (255 - 59) * intensity);
            const g = Math.floor(130 + (255 - 130) * intensity);
            const b = 246;
            
            ctx.fillStyle = `rgb(${r}, ${g}, ${b})`;
            ctx.fillRect(x, y, cellWidth, cellHeight);
            
            // Border
            ctx.strokeStyle = 'white';
            ctx.lineWidth = 1;
            ctx.strokeRect(x, y, cellWidth, cellHeight);
            
            // Text
            ctx.fillStyle = val > maxVal / 2 ? 'white' : 'black';
            ctx.font = 'bold 14px Arial';
            ctx.textAlign = 'center';
            ctx.textBaseline = 'middle';
            ctx.fillText(
                val.toString(),
                x + cellWidth / 2,
                y + cellHeight / 2
            );
        });
    });
    
    // Labels
    ctx.fillStyle = getComputedStyle(document.documentElement).getPropertyValue('--text-primary').trim() || '#1a1a1a';
    ctx.font = '12px Arial';
    for (let i = 0; i < size; i++) {
        ctx.fillText(`C${i}`, i * cellWidth + cellWidth / 2, canvas.height - 10);
        ctx.save();
        ctx.translate(10, i * cellHeight + cellHeight / 2);
        ctx.rotate(-Math.PI / 2);
        ctx.fillText(`C${i}`, 0, 0);
        ctx.restore();
    }
}

// ===== Training History =====
function showTrainingHistory() {
    switchView('metrics');
    document.querySelector('.nav-tab[onclick*="metrics"]').click();
}

function addToHistory() {
    if (state.errorData.length === 0) return;
    
    const history = {
        date: new Date().toLocaleString(),
        epochs: state.currentEpoch,
        finalError: state.errorData[state.errorData.length - 1].error,
        minError: Math.min(...state.errorData.map(d => d.error)),
        duration: state.metrics.trainingTime
    };
    
    state.trainingHistory.unshift(history);
    if (state.trainingHistory.length > 10) {
        state.trainingHistory.pop();
    }
    
    updateHistoryList();
}

function updateHistoryList() {
    const list = document.getElementById('historyList');
    if (state.trainingHistory.length === 0) {
        list.innerHTML = `
            <div class="history-item">
                <div class="history-header">
                    <span class="history-date">Aucun entraînement</span>
                </div>
                <div class="history-content">
                    <p>Lancez un entraînement pour voir l'historique</p>
                </div>
            </div>
        `;
        return;
    }
    
    list.innerHTML = state.trainingHistory.map(h => `
        <div class="history-item">
            <div class="history-header">
                <span class="history-date">${h.date}</span>
            </div>
            <div class="history-content">
                <p><strong>Époques:</strong> ${h.epochs} | <strong>Erreur finale:</strong> ${h.finalError.toFixed(4)} | <strong>Min:</strong> ${h.minError.toFixed(4)}</p>
                <p><strong>Durée:</strong> ${formatTime(h.duration)}</p>
            </div>
        </div>
    `).join('');
}

// ===== Dataset Functions =====
function showDatasetStats() {
    switchView('dataset');
    document.querySelector('.nav-tab[onclick*="dataset"]').click();
}

function previewDataset() {
    switchView('dataset');
    document.querySelector('.nav-tab[onclick*="dataset"]').click();
}

function updateDatasetView() {
    updateDatasetStats();
    updateDatasetPreview();
    updateDatasetInfo();
}

function updateDatasetStats() {
    const stats = document.getElementById('datasetStats');
    if (state.dataset) {
        stats.innerHTML = `
            <div class="metric-item">
                <span class="metric-label">Fichier:</span>
                <span class="metric-value">${state.dataset.path.split('/').pop()}</span>
            </div>
            <div class="metric-item">
                <span class="metric-label">Entrées:</span>
                <span class="metric-value">${state.dataset.inputs}</span>
            </div>
            <div class="metric-item">
                <span class="metric-label">Sorties:</span>
                <span class="metric-value">${state.dataset.outputs}</span>
            </div>
            <div class="metric-item">
                <span class="metric-label">Échantillons:</span>
                <span class="metric-value">~1000 (simulé)</span>
            </div>
        `;
    } else {
        stats.innerHTML = '<p class="info-placeholder">Chargez un dataset pour voir les statistiques</p>';
    }
}

function updateDatasetPreview() {
    const preview = document.getElementById('datasetPreview');
    if (state.dataset) {
        preview.innerHTML = `
            <div style="font-family: monospace; font-size: 11px; max-height: 200px; overflow-y: auto;">
                <div style="padding: 5px; background: var(--bg-tertiary); font-weight: bold;">Aperçu (premiers 5 lignes)</div>
                <div style="padding: 5px;">x1, x2, y</div>
                <div style="padding: 5px;">0.0, 0.0, 0.0</div>
                <div style="padding: 5px;">0.0, 1.0, 1.0</div>
                <div style="padding: 5px;">1.0, 0.0, 1.0</div>
                <div style="padding: 5px;">1.0, 1.0, 0.0</div>
            </div>
        `;
    } else {
        preview.innerHTML = '<p class="info-placeholder">Chargez un dataset pour voir l\'aperçu</p>';
    }
}

// ===== Weights Visualization =====
function showWeightsVisualization() {
    if (!state.network) {
        showNotification('Créez un réseau d\'abord', true);
        return;
    }
    showNotification('Visualisation des poids (fonctionnalité avancée)', 'info');
}

// ===== Export Functions =====
function exportResults() {
    if (state.errorData.length === 0) {
        showNotification('Aucune donnée à exporter', true);
        return;
    }
    showNotification('Export des résultats (fonctionnalité à implémenter)', 'info');
}

function exportNetwork() {
    if (!state.network) {
        showNotification('Aucun réseau à exporter', true);
        return;
    }
    showNotification('Export du réseau (fonctionnalité à implémenter)', 'info');
}

function exportChart() {
    if (state.errorData.length === 0) {
        showNotification('Aucun graphique à exporter', true);
        return;
    }
    showNotification('Export du graphique (fonctionnalité à implémenter)', 'info');
}

function clearChart() {
    if (confirm('Effacer le graphique ?')) {
        state.errorData = [];
        updateErrorChart();
        addLog('Graphique effacé', 'info');
    }
}

// ===== Help =====
function showHelp() {
    showNotification('Aide contextuelle (fonctionnalité à implémenter)', 'info');
}

// ===== Enhanced Training =====
function startTraining() {
    if (!state.network) {
        showNotification('Veuillez créer un réseau d\'abord', true);
        return;
    }
    
    if (!state.dataset) {
        showNotification('Veuillez charger un dataset d\'abord', true);
        return;
    }
    
    if (state.isTraining) {
        return;
    }
    
    state.isTraining = true;
    state.currentEpoch = 0;
    state.errorData = [];
    state.startTime = Date.now();
    state.metrics.trainingTime = 0;
    
    document.getElementById('btnStartTraining').disabled = true;
    const stopBtn = document.getElementById('btnStopTraining');
    stopBtn.disabled = false;
    stopBtn.classList.add('stop');
    document.getElementById('progressContainer').style.display = 'block';
    
    updateTrainingInfo();
    updateStatus();
    addLog('Entraînement démarré', 'success');
    
    // Simulate training
    state.trainingInterval = setInterval(() => {
        state.currentEpoch++;
        const error = Math.max(0.01, 1 - (state.currentEpoch / state.maxEpochs) + Math.random() * 0.1);
        state.errorData.push({ epoch: state.currentEpoch, error: error });
        
        state.metrics.trainingTime = Math.floor((Date.now() - state.startTime) / 1000);
        
        updateProgress();
        updateErrorChart();
        updateTrainingInfo();
        updateDashboard();
        updateMetrics();
        
        // Update training view stats
        document.getElementById('trainingEpoch').textContent = state.currentEpoch;
        document.getElementById('trainingError').textContent = error.toFixed(4);
        
        if (state.currentEpoch >= state.maxEpochs) {
            stopTraining();
            addLog('Entraînement terminé avec succès', 'success');
            addToHistory();
            
            // Generate confusion matrix for demo
            state.confusionMatrix = [
                [45, 5],
                [3, 47]
            ];
            state.metrics.accuracy = 0.92;
            drawConfusionMatrix();
            updateMetrics();
        }
    }, 100);
}

function stopTraining() {
    if (!state.isTraining) {
        return;
    }
    
    state.isTraining = false;
    
    if (state.trainingInterval) {
        clearInterval(state.trainingInterval);
        state.trainingInterval = null;
    }
    
    document.getElementById('btnStartTraining').disabled = false;
    const stopBtn = document.getElementById('btnStopTraining');
    stopBtn.disabled = true;
    stopBtn.classList.remove('stop');
    
    if (state.errorData.length > 0) {
        addToHistory();
    }
    
    updateStatus();
    addLog('Entraînement arrêté', 'warning');
}

// ===== Activity Tracking =====
function addActivity(message) {
    const list = document.getElementById('activityList');
    const time = new Date().toLocaleTimeString();
    
    const item = document.createElement('div');
    item.className = 'activity-item';
    item.innerHTML = `
        <i class="fas fa-info-circle"></i>
        <span>${message}</span>
        <span class="activity-time">${time}</span>
    `;
    
    list.insertBefore(item, list.firstChild);
    if (list.children.length > 5) {
        list.removeChild(list.lastChild);
    }
}

// Initialize
document.addEventListener('DOMContentLoaded', () => {
    updateNetworkInfo();
    updateDatasetInfo();
    updateTrainingInfo();
    updateStatus();
    updateDashboard();
    drawNetwork();
    updateErrorChart();
    
    addLog('Application initialisée', 'info');
    addActivity('Application démarrée');
});

