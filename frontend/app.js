const canvas = document.getElementById("game");
const ctx = canvas.getContext("2d");

const ui = {
  score: document.getElementById("score"),
  best: document.getElementById("best"),
  level: document.getElementById("level"),
  length: document.getElementById("length"),
  overlay: document.getElementById("overlay"),
  overlayTitle: document.getElementById("overlayTitle"),
  overlayText: document.getElementById("overlayText"),
  startBtn: document.getElementById("startBtn"),
  pauseBtn: document.getElementById("pauseBtn"),
  restartBtn: document.getElementById("restartBtn"),
};

const board = { cols: 36, rows: 20 };
let cell = 25;
let offsetX = 0;
let offsetY = 0;
let state;
let lastTime = 0;
let accumulator = 0;

function newGame(start = false) {
  const mid = { x: Math.floor(board.cols / 2), y: Math.floor(board.rows / 2) };
  state = {
    snake: [
      { x: mid.x, y: mid.y },
      { x: mid.x - 1, y: mid.y },
      { x: mid.x - 2, y: mid.y },
      { x: mid.x - 3, y: mid.y },
    ],
    dir: { x: 1, y: 0 },
    nextDir: { x: 1, y: 0 },
    food: { x: 6, y: 6, type: "normal" },
    score: 0,
    best: Number(localStorage.getItem("snakeDesktopBest") || 0),
    level: 1,
    speed: 155,
    boost: 0,
    running: start,
    paused: false,
    over: false,
    win: false,
    particles: [],
  };
  placeFood();
  updateHud();
  updateOverlay();
}

function resizeCanvas() {
  const rect = canvas.getBoundingClientRect();
  const dpr = window.devicePixelRatio || 1;
  canvas.width = Math.floor(rect.width * dpr);
  canvas.height = Math.floor(rect.height * dpr);
  ctx.setTransform(dpr, 0, 0, dpr, 0, 0);
  cell = Math.min(rect.width / board.cols, rect.height / board.rows);
  offsetX = (rect.width - board.cols * cell) / 2;
  offsetY = (rect.height - board.rows * cell) / 2;
}

function placeFood() {
  const empty = [];
  for (let y = 0; y < board.rows; y++) {
    for (let x = 0; x < board.cols; x++) {
      if (!state.snake.some((part) => part.x === x && part.y === y)) {
        empty.push({ x, y });
      }
    }
  }
  if (empty.length === 0) {
    state.win = true;
    state.running = false;
    updateOverlay();
    return;
  }
  const chosen = empty[Math.floor(Math.random() * empty.length)];
  state.food = {
    ...chosen,
    type: Math.random() < 0.14 ? "bonus" : "normal",
  };
}

function startGame() {
  if (state.over || state.win) {
    newGame(true);
    return;
  }
  state.running = true;
  state.paused = false;
  updateOverlay();
}

function togglePause() {
  if (!state.running || state.over || state.win) return;
  state.paused = !state.paused;
  updateOverlay();
}

function boost() {
  if (!state.running || state.paused || state.over || state.snake.length <= 4 || state.boost > 0) return;
  state.snake.pop();
  state.boost = 24;
}

function setDirection(x, y) {
  if (state.dir.x + x === 0 && state.dir.y + y === 0) return;
  state.nextDir = { x, y };
  startGame();
}

function step() {
  if (!state.running || state.paused || state.over || state.win) return;

  state.dir = state.nextDir;
  const head = state.snake[0];
  const next = { x: head.x + state.dir.x, y: head.y + state.dir.y };
  const eating = next.x === state.food.x && next.y === state.food.y;
  const collisionBody = state.snake.slice(0, eating ? state.snake.length : state.snake.length - 1);

  if (
    next.x < 0 ||
    next.y < 0 ||
    next.x >= board.cols ||
    next.y >= board.rows ||
    collisionBody.some((part) => part.x === next.x && part.y === next.y)
  ) {
    state.over = true;
    state.running = false;
    saveBest();
    updateOverlay();
    return;
  }

  state.snake.unshift(next);
  if (eating) {
    const points = state.food.type === "bonus" ? 30 : 10;
    state.score += points;
    state.level = Math.floor(state.score / 50) + 1;
    state.speed = Math.max(65, 155 - (state.level - 1) * 9);
    burst(state.food.x, state.food.y, state.food.type);
    if (state.food.type === "bonus") {
      state.snake.push({ ...state.snake[state.snake.length - 1] });
    }
    placeFood();
  } else {
    state.snake.pop();
  }

  if (state.boost > 0) state.boost--;
  saveBest();
  updateHud();
}

function saveBest() {
  if (state.score > state.best) {
    state.best = state.score;
    localStorage.setItem("snakeDesktopBest", String(state.best));
  }
}

function burst(x, y, type) {
  const color = type === "bonus" ? "#f4c542" : "#e75f55";
  for (let i = 0; i < 14; i++) {
    state.particles.push({
      x: x + 0.5,
      y: y + 0.5,
      vx: (Math.random() - 0.5) * 0.35,
      vy: (Math.random() - 0.5) * 0.35,
      life: 22,
      color,
    });
  }
}

function updateHud() {
  ui.score.textContent = state.score;
  ui.best.textContent = state.best;
  ui.level.textContent = state.level;
  ui.length.textContent = state.snake.length;
}

function updateOverlay() {
  if (state.running && !state.paused) {
    ui.overlay.classList.add("hidden");
    return;
  }
  ui.overlay.classList.remove("hidden");
  if (state.over) {
    ui.overlayTitle.textContent = "游戏结束";
    ui.overlayText.textContent = "按 R 重开，或点击下方重开按钮";
  } else if (state.win) {
    ui.overlayTitle.textContent = "胜利";
    ui.overlayText.textContent = "整张地图已经被填满";
  } else if (state.paused) {
    ui.overlayTitle.textContent = "已暂停";
    ui.overlayText.textContent = "按空格继续游戏";
  } else {
    ui.overlayTitle.textContent = "准备开始";
    ui.overlayText.textContent = "按空格或 Enter 开始游戏";
  }
}

function boardX(x) {
  return offsetX + x * cell;
}

function boardY(y) {
  return offsetY + y * cell;
}

function drawBoard() {
  const rect = canvas.getBoundingClientRect();
  const width = board.cols * cell;
  const height = board.rows * cell;
  ctx.fillStyle = "#9ed76e";
  ctx.fillRect(0, 0, rect.width, rect.height);
  ctx.fillStyle = "#aadf7b";
  ctx.fillRect(offsetX, offsetY, width, height);

  for (let y = 0; y < board.rows; y++) {
    for (let x = 0; x < board.cols; x++) {
      ctx.fillStyle = (x + y) % 2 === 0 ? "#afe486" : "#a5dc75";
      ctx.fillRect(boardX(x), boardY(y), cell, cell);
    }
  }

  ctx.fillStyle = "rgba(255,255,255,0.12)";
  for (let i = 0; i < 45; i++) {
    const x = boardX((i * 137) % board.cols) + cell * 0.32;
    const y = boardY((i * 73) % board.rows) + cell * 0.34;
    ctx.beginPath();
    ctx.arc(x, y, Math.max(1.4, cell * 0.055), 0, Math.PI * 2);
    ctx.fill();
  }
}

function drawFood(time) {
  const x = boardX(state.food.x + 0.5);
  const y = boardY(state.food.y + 0.5);
  const radius = (state.food.type === "bonus" ? 0.36 : 0.29) * cell;
  const pulse = 1 + Math.sin(time / 180) * 0.06;

  ctx.save();
  ctx.translate(x, y);
  ctx.scale(pulse, pulse);
  ctx.fillStyle = state.food.type === "bonus" ? "#f4c542" : "#e75f55";
  ctx.shadowColor = "rgba(58, 67, 43, 0.22)";
  ctx.shadowBlur = 8;
  ctx.beginPath();
  ctx.arc(0, 0, radius, 0, Math.PI * 2);
  ctx.fill();

  ctx.shadowBlur = 0;
  ctx.fillStyle = "rgba(255,255,255,0.7)";
  ctx.beginPath();
  ctx.arc(-radius * 0.28, -radius * 0.33, radius * 0.28, 0, Math.PI * 2);
  ctx.fill();
  ctx.restore();
}

function drawSnake() {
  for (let i = state.snake.length - 1; i >= 0; i--) {
    const part = state.snake[i];
    const x = boardX(part.x + 0.5);
    const y = boardY(part.y + 0.5);
    const radius = (i === 0 ? 0.47 : 0.42) * cell;
    const gradient = ctx.createRadialGradient(x - radius * 0.25, y - radius * 0.3, radius * 0.12, x, y, radius);
    gradient.addColorStop(0, "#f5ffdc");
    gradient.addColorStop(0.28, i === 0 ? "#72d96d" : "#59bf5e");
    gradient.addColorStop(1, "#2f7d44");

    ctx.fillStyle = gradient;
    ctx.shadowColor = "rgba(52, 84, 44, 0.24)";
    ctx.shadowBlur = i === 0 ? 9 : 5;
    ctx.beginPath();
    ctx.arc(x, y, radius, 0, Math.PI * 2);
    ctx.fill();
  }
  ctx.shadowBlur = 0;
  drawEyes();
}

function drawEyes() {
  const head = state.snake[0];
  const hx = boardX(head.x + 0.5);
  const hy = boardY(head.y + 0.5);
  const forwardX = state.dir.x * cell * 0.16;
  const forwardY = state.dir.y * cell * 0.16;
  const sideX = state.dir.y * cell * 0.12;
  const sideY = -state.dir.x * cell * 0.12;
  drawEye(hx + forwardX + sideX, hy + forwardY + sideY);
  drawEye(hx + forwardX - sideX, hy + forwardY - sideY);
}

function drawEye(x, y) {
  ctx.fillStyle = "#fff";
  ctx.beginPath();
  ctx.arc(x, y, cell * 0.1, 0, Math.PI * 2);
  ctx.fill();
  ctx.fillStyle = "#273026";
  ctx.beginPath();
  ctx.arc(x + cell * 0.02, y + cell * 0.02, cell * 0.045, 0, Math.PI * 2);
  ctx.fill();
}

function drawParticles() {
  state.particles = state.particles.filter((p) => p.life > 0);
  state.particles.forEach((p) => {
    p.x += p.vx;
    p.y += p.vy;
    p.life--;
    ctx.globalAlpha = p.life / 22;
    ctx.fillStyle = p.color;
    ctx.beginPath();
    ctx.arc(boardX(p.x), boardY(p.y), cell * 0.1, 0, Math.PI * 2);
    ctx.fill();
  });
  ctx.globalAlpha = 1;
}

function draw(time) {
  drawBoard();
  drawFood(time);
  drawSnake();
  drawParticles();
}

function loop(time) {
  const delta = time - lastTime;
  lastTime = time;
  accumulator += delta;
  const delay = state.boost > 0 ? Math.max(48, state.speed - 70) : state.speed;
  while (accumulator >= delay) {
    step();
    accumulator -= delay;
  }
  draw(time);
  requestAnimationFrame(loop);
}

function bind() {
  window.addEventListener("resize", () => {
    resizeCanvas();
    draw(performance.now());
  });

  window.addEventListener("keydown", (event) => {
    const key = event.key.toLowerCase();
    const directions = {
      arrowup: [0, -1],
      w: [0, -1],
      arrowdown: [0, 1],
      s: [0, 1],
      arrowleft: [-1, 0],
      a: [-1, 0],
      arrowright: [1, 0],
      d: [1, 0],
    };

    if (directions[key]) {
      event.preventDefault();
      setDirection(...directions[key]);
    } else if (key === " " || key === "enter") {
      event.preventDefault();
      if (!state.running || state.paused) {
        if (state.paused) togglePause();
        else startGame();
      } else {
        togglePause();
      }
    } else if (key === "r") {
      newGame(true);
    } else if (key === "b") {
      boost();
    }
  });

  ui.startBtn.addEventListener("click", startGame);
  ui.pauseBtn.addEventListener("click", togglePause);
  ui.restartBtn.addEventListener("click", () => newGame(true));
}

newGame(false);
resizeCanvas();
bind();
draw(0);
requestAnimationFrame(loop);
