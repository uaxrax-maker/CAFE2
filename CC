<!DOCTYPE html>
<html lang="ar" dir="rtl">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>نظام إدارة الكافيه الذكي - POS & QR</title>
    <!-- Tailwind CSS للتصميم -->
    <script src="https://cdn.tailwindcss.com"></script>
    <!-- خط تجوال العربي -->
    <link href="https://fonts.googleapis.com/css2?family=Tajawal:wght@400;500;700;800&display=swap" rel="stylesheet">
    <!-- أيقونات FontAwesome -->
    <link rel="stylesheet" href="https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.4.0/css/all.min.css">
    <style>
        body { font-family: 'Tajawal', sans-serif; background-color: #f3f4f6; }
        .active-tab { background-color: #1f2937; color: #ffffff; }
        .active-cat { background-color: #d97706; color: white; }
        @media print {
            body * { visibility: hidden; }
            #print-area, #print-area * { visibility: visible; }
            #print-area { position: absolute; left: 0; top: 0; width: 100%; }
        }
    </style>
</head>
<body class="h-screen flex flex-col overflow-hidden">

    <!-- الشريط العلوي -->
    <header class="bg-gray-900 text-white px-6 py-3 flex justify-between items-center shadow-md z-10">
        <div class="flex items-center gap-3">
            <div class="bg-amber-600 p-2 rounded-lg text-white">
                <i class="fa-solid fa-mug-hot text-xl"></i>
            </div>
            <div>
                <h1 id="header-cafe-name" class="text-xl font-bold tracking-wide">اسم الكافيه</h1>
                <p id="header-cafe-sub" class="text-xs text-gray-400">نظام نقاط البيع والطلب الذاتي</p>
            </div>
        </div>

        <!-- أزرار التنقل بين الشاشات -->
        <nav class="flex bg-gray-800 rounded-lg p-1 gap-1 overflow-x-auto">
            <button onclick="switchTab('pos')" id="tab-pos" class="tab-btn active-tab px-4 py-2 rounded-md text-sm font-medium transition flex items-center gap-2">
                <i class="fa-solid fa-cash-register"></i> الكاشير
            </button>
            <button onclick="switchTab('tables')" id="tab-tables" class="tab-btn px-4 py-2 rounded-md text-sm font-medium text-gray-300 hover:text-white transition flex items-center gap-2">
                <i class="fa-solid fa-chair"></i> الطاولات <span id="busy-tables-badge" class="bg-red-500 text-white text-xs px-1.5 py-0.5 rounded-full hidden">0</span>
            </button>
            <button onclick="switchTab('qr')" id="tab-qr" class="tab-btn px-4 py-2 rounded-md text-sm font-medium text-gray-300 hover:text-white transition flex items-center gap-2">
                <i class="fa-solid fa-qrcode"></i> الباركود وشاشة العميل
            </button>
            <button onclick="switchTab('orders')" id="tab-orders" class="tab-btn px-4 py-2 rounded-md text-sm font-medium text-gray-300 hover:text-white transition flex items-center gap-2">
                <i class="fa-solid fa-receipt"></i> المبيعات
            </button>
            <button onclick="switchTab('settings')" id="tab-settings" class="tab-btn px-4 py-2 rounded-md text-sm font-medium text-gray-300 hover:text-white transition flex items-center gap-2">
                <i class="fa-solid fa-gear"></i> الإعدادات والمنيو
            </button>
        </nav>
    </header>

    <!-- المحتوى الرئيسي -->
    <main class="flex-1 overflow-hidden relative">

        <!-- 1. شاشة الكاشير (POS) -->
        <section id="view-pos" class="h-full flex flex-col md:flex-row">
            <!-- قائمة الأصناف -->
            <div class="flex-1 flex flex-col h-full overflow-hidden p-4">
                <!-- شريط الفئات -->
                <div id="pos-categories" class="flex gap-2 pb-3 overflow-x-auto border-b border-gray-200"></div>
                <!-- شبكة المنتجات -->
                <div id="pos-items" class="grid grid-cols-2 sm:grid-cols-3 lg:grid-cols-4 gap-4 py-4 overflow-y-auto flex-1"></div>
            </div>

            <!-- سلة المشتريات والفاتورة -->
            <div class="w-full md:w-96 bg-white border-r border-gray-200 flex flex-col h-full shadow-lg">
                <div class="p-4 bg-gray-50 border-b flex justify-between items-center">
                    <h2 class="font-bold text-gray-800 flex items-center gap-2">
                        <i class="fa-solid fa-basket-shopping text-amber-600"></i> تفاصيل الطلب الحالي
                    </h2>
                    <button onclick="clearCart()" class="text-xs text-red-500 hover:text-red-700 font-medium">
                        <i class="fa-solid fa-trash"></i> إفراغ
                    </button>
                </div>

                <!-- اختيار نوع الطلب ورقم الطاولة -->
                <div class="p-3 bg-gray-100 border-b grid grid-cols-2 gap-2 text-sm">
                    <div>
                        <label class="block text-xs text-gray-500 mb-1">نوع الطلب</label>
                        <select id="order-type" onchange="toggleTableSelect()" class="w-full p-2 rounded border border-gray-300 bg-white font-medium">
                            <option value="table">محلي (طاولة)</option>
                            <option value="takeaway">سفري (خارجي)</option>
                        </select>
                    </div>
                    <div id="table-select-wrapper">
                        <label class="block text-xs text-gray-500 mb-1">اختر الطاولة</label>
                        <select id="order-table" class="w-full p-2 rounded border border-gray-300 bg-white font-medium"></select>
                    </div>
                </div>

                <!-- قائمة العناصر في السلة -->
                <div id="cart-items" class="flex-1 overflow-y-auto p-4 space-y-3"></div>

                <!-- الإجمالي وأزرار الدفع -->
                <div class="p-4 bg-gray-50 border-t space-y-3">
                    <div class="flex gap-2">
                        <input type="text" id="customer-name" placeholder="اسم العميل (اختياري)" class="w-1/2 p-2 text-sm border rounded">
                        <input type="tel" id="customer-phone" placeholder="رقم الجوال للواتساب" class="w-1/2 p-2 text-sm border rounded">
                    </div>

                    <div class="flex justify-between items-center text-lg font-bold text-gray-900 pt-2 border-t">
                        <span>الإجمالي:</span>
                        <span id="cart-total" class="text-amber-600 text-xl">0.000 ر.ع</span>
                    </div>

                    <div class="grid grid-cols-2 gap-2 pt-1">
                        <button onclick="submitOrder('hold')" class="bg-amber-500 hover:bg-amber-600 text-white py-2.5 rounded-lg font-bold text-sm transition flex items-center justify-center gap-2">
                            <i class="fa-solid fa-clock"></i> تعليق للطاولة
                        </button>
                        <button onclick="submitOrder('paid')" class="bg-emerald-600 hover:bg-emerald-700 text-white py-2.5 rounded-lg font-bold text-sm transition flex items-center justify-center gap-2">
                            <i class="fa-solid fa-check-double"></i> دفع وإغلاق
                        </button>
                    </div>
                    <div class="grid grid-cols-2 gap-2">
                        <button onclick="sendWhatsApp()" class="bg-green-500 hover:bg-green-600 text-white py-2 rounded-lg font-medium text-xs transition flex items-center justify-center gap-1">
                            <i class="fa-brands fa-whatsapp text-base"></i> إرسال واتساب
                        </button>
                        <button onclick="printReceipt()" class="bg-gray-700 hover:bg-gray-800 text-white py-2 rounded-lg font-medium text-xs transition flex items-center justify-center gap-1">
                            <i class="fa-solid fa-print"></i> طباعة فاتورة
                        </button>
                    </div>
                </div>
            </div>
        </section>

        <!-- 2. شاشة إدارة الطاولات -->
        <section id="view-tables" class="h-full p-6 overflow-y-auto hidden">
            <div class="max-w-6xl mx-auto">
                <div class="flex justify-between items-center mb-6">
                    <div>
                        <h2 class="text-2xl font-bold text-gray-800">خريطة الطاولات والطلبات النشطة</h2>
                        <p class="text-sm text-gray-500">اضغط على أي طاولة مشغولة لعرض تفاصيل طلبها أو إتمام الدفع</p>
                    </div>
                    <div class="flex gap-4 text-sm">
                        <span class="flex items-center gap-2"><span class="w-3 h-3 rounded-full bg-emerald-500 inline-block"></span> متاحة</span>
                        <span class="flex items-center gap-2"><span class="w-3 h-3 rounded-full bg-red-500 inline-block"></span> مشغولة (طلب مفتوح)</span>
                    </div>
                </div>
                <div id="tables-grid" class="grid grid-cols-2 sm:grid-cols-3 md:grid-cols-4 lg:grid-cols-5 gap-4"></div>
            </div>
        </section>

        <!-- 3. شاشة الباركود (QR) ومحاكي طلب العميل -->
        <section id="view-qr" class="h-full p-6 overflow-y-auto hidden">
            <div class="max-w-6xl mx-auto grid grid-cols-1 lg:grid-cols-2 gap-8">
                <!-- قسم توليد الباركود للطاولات -->
                <div class="bg-white p-6 rounded-xl shadow">
                    <h2 class="text-xl font-bold text-gray-800 mb-2 flex items-center gap-2">
                        <i class="fa-solid fa-qrcode text-amber-600"></i> رموز الباركود (QR) للطاولات
                    </h2>
                    <p class="text-sm text-gray-500 mb-4">اطبع هذه الرموز وضعها على الطاولات. عند مسحها يفتح المنيو برقم الطاولة تلقائياً.</p>
                    
                    <div id="qr-cards-container" class="grid grid-cols-2 sm:grid-cols-3 gap-4 max-h-96 overflow-y-auto p-2 border rounded-lg"></div>
                </div>

                <!-- محاكي شاشة جوال العميل (الطلب الذاتي) -->
                <div class="flex flex-col items-center">
                    <div class="mb-2 text-center">
                        <span class="bg-amber-100 text-amber-800 text-xs font-bold px-3 py-1 rounded-full">معاينة حية لشاشة العميل (الطلب الذاتي)</span>
                    </div>
                    <div class="w-80 bg-white border-8 border-gray-800 rounded-3xl shadow-2xl overflow-hidden flex flex-col h-[520px]">
                        <!-- هيدر جوال العميل -->
                        <div class="bg-amber-600 text-white p-4 text-center">
                            <h3 id="qr-sim-cafe-name" class="font-bold text-lg">اسم الكافيه</h3>
                            <div class="flex justify-center items-center gap-2 mt-1">
                                <span class="text-xs bg-amber-700 px-2 py-0.5 rounded">أنت تطلب للطاولة رقم:</span>
                                <select id="qr-sim-table" class="text-gray-900 text-xs font-bold rounded px-1 py-0.5"></select>
                            </div>
                        </div>
                        <!-- قائمة الأصناف في جوال العميل -->
                        <div id="qr-sim-menu" class="flex-1 overflow-y-auto p-3 space-y-2 bg-gray-50"></div>
                        <!-- فوتر إرسال الطلب من جوال العميل -->
                        <div class="p-3 bg-white border-t">
                            <input type="text" id="qr-sim-note" placeholder="ملاحظاتك (مثل: سكر قليل، حليب لوز)..." class="w-full text-xs p-2 border rounded mb-2">
                            <button onclick="submitCustomerQROrder()" class="w-full bg-gray-900 hover:bg-gray-800 text-white py-2 rounded-lg text-sm font-bold flex justify-between px-4 items-center">
                                <span>إرسال الطلب للكاشير</span>
                                <span id="qr-sim-total">0.000 ر.ع</span>
                            </button>
                        </div>
                    </div>
                </div>
            </div>
        </section>

        <!-- 4. شاشة سجل المبيعات والتقارير -->
        <section id="view-orders" class="h-full p-6 overflow-y-auto hidden">
            <div class="max-w-5xl mx-auto space-y-6">
                <div class="grid grid-cols-1 sm:grid-cols-3 gap-4">
                    <div class="bg-white p-5 rounded-xl shadow border-r-4 border-emerald-500">
                        <p class="text-gray-500 text-sm">إجمالي المبيعات المدفوعة</p>
                        <h3 id="stat-total-sales" class="text-2xl font-bold text-gray-800 mt-1">0.000 ر.ع</h3>
                    </div>
                    <div class="bg-white p-5 rounded-xl shadow border-r-4 border-blue-500">
                        <p class="text-gray-500 text-sm">عدد الفواتير المكتملة</p>
                        <h3 id="stat-total-orders" class="text-2xl font-bold text-gray-800 mt-1">0</h3>
                    </div>
                    <div class="bg-white p-5 rounded-xl shadow border-r-4 border-amber-500">
                        <p class="text-gray-500 text-sm">طلبات قيد الانتظار (طاولات)</p>
                        <h3 id="stat-active-tables" class="text-2xl font-bold text-gray-800 mt-1">0</h3>
                    </div>
                </div>

                <div class="bg-white rounded-xl shadow overflow-hidden">
                    <div class="p-4 border-b flex justify-between items-center">
                        <h3 class="font-bold text-gray-800">سجل الفواتير المكتملة</h3>
                        <button onclick="clearSalesHistory()" class="text-xs text-red-500 hover:underline">تصفير السجل</button>
                    </div>
                    <div class="overflow-x-auto">
                        <table class="w-full text-right text-sm">
                            <thead class="bg-gray-50 text-gray-600">
                                <tr>
                                    <th class="p-3">رقم الفاتورة</th>
                                    <th class="p-3">النوع / الطاولة</th>
                                    <th class="p-3">الأصناف</th>
                                    <th class="p-3">الوقت</th>
                                    <th class="p-3">الإجمالي</th>
                                </tr>
                            </thead>
                            <tbody id="orders-history-body" class="divide-y"></tbody>
                        </table>
                    </div>
                </div>
            </div>
        </section>

        <!-- 5. شاشة الإعدادات وتخصيص الاسم والمنيو -->
        <section id="view-settings" class="h-full p-6 overflow-y-auto hidden">
            <div class="max-w-4xl mx-auto space-y-6">
                <!-- إعدادات الهوية واسم الكافيه -->
                <div class="bg-white p-6 rounded-xl shadow">
                    <h2 class="text-lg font-bold text-gray-800 mb-4 border-b pb-2 flex items-center gap-2">
                        <i class="fa-solid fa-store text-amber-600"></i> إعدادات هوية الكافيه والطاولات
                    </h2>
                    <div class="grid grid-cols-1 md:grid-cols-3 gap-4">
                        <div>
                            <label class="block text-sm font-medium text-gray-700 mb-1">اسم الكافيه (يظهر في كل النظام)</label>
                            <input type="text" id="setting-cafe-name" class="w-full p-2 border rounded-lg">
                        </div>
                        <div>
                            <label class="block text-sm font-medium text-gray-700 mb-1">الوصف الفرعي / الترحيب</label>
                            <input type="text" id="setting-cafe-sub" class="w-full p-2 border rounded-lg">
                        </div>
                        <div>
                            <label class="block text-sm font-medium text-gray-700 mb-1">عدد الطاولات في الكافيه</label>
                            <input type="number" id="setting-tables-count" min="1" max="50" class="w-full p-2 border rounded-lg">
                        </div>
                    </div>
                    <button onclick="saveCafeSettings()" class="mt-4 bg-gray-900 hover:bg-gray-800 text-white px-6 py-2 rounded-lg text-sm font-bold transition">
                        حفظ إعدادات الكافيه
                    </button>
                </div>

                <!-- إضافة صنف جديد للمنيو -->
                <div class="bg-white p-6 rounded-xl shadow">
                    <h2 class="text-lg font-bold text-gray-800 mb-4 border-b pb-2 flex items-center gap-2">
                        <i class="fa-solid fa-plus-circle text-amber-600"></i> إضافة صنف جديد للقائمة
                    </h2>
                    <div class="grid grid-cols-1 md:grid-cols-4 gap-4">
                        <input type="text" id="new-item-name" placeholder="اسم الصنف (مثال: سبانيش لاتيه)" class="p-2 border rounded-lg">
                        <input type="number" id="new-item-price" step="0.100" placeholder="السعر بالريال (مثال: 1.800)" class="p-2 border rounded-lg">
                        <select id="new-item-cat" class="p-2 border rounded-lg">
                            <option value="قهوة ساخنة">قهوة ساخنة</option>
                            <option value="مشروبات باردة">مشروبات باردة</option>
                            <option value="قهوة مختصة (V60)">قهوة مختصة (V60)</option>
                            <option value="حلويات ومخبوزات">حلويات ومخبوزات</option>
                        </select>
                        <button onclick="addNewMenuItem()" class="bg-amber-600 hover:bg-amber-700 text-white font-bold rounded-lg p-2 transition">
                            + إضافة للمنيو
                        </button>
                    </div>
                </div>

                <!-- إدارة الأصناف الحالية -->
                <div class="bg-white p-6 rounded-xl shadow">
                    <h2 class="text-lg font-bold text-gray-800 mb-4 border-b pb-2">الأصناف الحالية</h2>
                    <div id="settings-menu-list" class="divide-y max-h-80 overflow-y-auto"></div>
                </div>
            </div>
        </section>

    </main>

    <!-- منطقة الطباعة المخفية للفاتورة الحرارية -->
    <div id="print-area" class="hidden p-4 text-center font-mono text-sm">
        <h2 id="print-cafe-name" class="text-xl font-bold"></h2>
        <p id="print-order-info" class="text-xs my-1"></p>
        <hr class="my-2 border-dashed border-gray-400">
        <div id="print-items" class="text-right space-y-1"></div>
        <hr class="my-2 border-dashed border-gray-400">
        <div class="flex justify-between font-bold text-base">
            <span>الإجمالي:</span>
            <span id="print-total"></span>
        </div>
        <p class="text-xs mt-4">شكراً لزيارتكم!</p>
    </div>

    <!-- البرمجة والتفاعل (JavaScript) -->
    <script>
        // البيانات الافتراضية القابلة للتعديل
        const defaultData = {
            cafeName: "كافيه القهوة المختصة",
            cafeSub: "نظام نقاط البيع والطلب الذاتي",
            tablesCount: 10,
            categories: ["الكل", "قهوة ساخنة", "مشروبات باردة", "قهوة مختصة (V60)", "حلويات ومخبوزات"],
            menu: [
                { id: 1, name: "إسبريسو دبل", price: 1.200, category: "قهوة ساخنة" },
                { id: 2, name: "كابتشينو", price: 1.600, category: "قهوة ساخنة" },
                { id: 3, name: "فلات وايت", price: 1.500, category: "قهوة ساخنة" },
                { id: 4, name: "سبانيش لاتيه بارد", price: 1.900, category: "مشروبات باردة" },
                { id: 5, name: "آيس أمريكانو", price: 1.400, category: "مشروبات باردة" },
                { id: 6, name: "موهيتو باشن فروت", price: 1.800, category: "مشروبات باردة" },
                { id: 7, name: "قهوة مقطرة V60 إثيوبي", price: 2.200, category: "قهوة مختصة (V60)" },
                { id: 8, name: "كيمكس كولومبي", price: 2.400, category: "قهوة مختصة (V60)" },
                { id: 9, name: "كيكة سان سيباستيان", price: 2.500, category: "حلويات ومخبوزات" },
                { id: 10, name: "كرواسون زبدة طازج", price: 1.100, category: "حلويات ومخبوزات" }
            ],
            activeTableOrders: {}, // { tableNumber: { items: [], note: '', customer: '' } }
            completedOrders: []
        };

        // تحميل البيانات من الذاكرة المحلية (LocalStorage)
        let appData = JSON.parse(localStorage.getItem('custom_cafe_pos_data')) || defaultData;
        let currentCategory = "الكل";
        let currentCart = [];
        let qrSimCart = [];

        function saveData() {
            localStorage.setItem('custom_cafe_pos_data', JSON.stringify(appData));
            updateUI();
        }

        // تحديث واجهة المستخدم بالكامل
        function updateUI() {
            // تحديث الاسم في الهيدر والمحاكي والإعدادات
            document.getElementById('header-cafe-name').innerText = appData.cafeName;
            document.getElementById('header-cafe-sub').innerText = appData.cafeSub;
            document.getElementById('qr-sim-cafe-name').innerText = appData.cafeName;
            document.getElementById('setting-cafe-name').value = appData.cafeName;
            document.getElementById('setting-cafe-sub').value = appData.cafeSub;
            document.getElementById('setting-tables-count').value = appData.tablesCount;
            document.title = appData.cafeName + " - نظام الكاشير";

            renderCategories();
            renderMenuItems();
            renderTableSelects();
            renderCart();
            renderTablesMap();
            renderQRSection();
            renderOrdersHistory();
            renderSettingsMenu();
        }

        // التنقل بين التبويبات
        function switchTab(tabName) {
            ['pos', 'tables', 'qr', 'orders', 'settings'].forEach(t => {
                document.getElementById('view-' + t).classList.add('hidden');
                document.getElementById('tab-' + t).classList.remove('active-tab');
                document.getElementById('tab-' + t).classList.add('text-gray-300');
            });
            document.getElementById('view-' + tabName).classList.remove('hidden');
            const activeBtn = document.getElementById('tab-' + tabName);
            activeBtn.classList.add('active-tab');
            activeBtn.classList.remove('text-gray-300');
        }

        // عرض الفئات في الكاشير
        function renderCategories() {
            const container = document.getElementById('pos-categories');
            container.innerHTML = appData.categories.map(cat => `
                <button onclick="setCategory('${cat}')" class="px-4 py-2 rounded-full text-sm font-bold whitespace-nowrap transition ${currentCategory === cat ? 'active-cat shadow' : 'bg-white text-gray-700 hover:bg-gray-200'}">
                    ${cat}
                </button>
            `).join('');
        }

        function setCategory(cat) {
            currentCategory = cat;
            renderCategories();
            renderMenuItems();
        }

        // عرض الأصناف في الكاشير
        function renderMenuItems() {
            const container = document.getElementById('pos-items');
            const filtered = currentCategory === "الكل" ? appData.menu : appData.menu.filter(i => i.category === currentCategory);
            container.innerHTML = filtered.map(item => `
                <div onclick="addToCart(${item.id})" class="bg-white p-4 rounded-xl shadow-sm hover:shadow-md border border-gray-100 cursor-pointer transition flex flex-col justify-between active:scale-95">
                    <div>
                        <span class="text-xs text-amber-600 bg-amber-50 px-2 py-0.5 rounded font-medium">${item.category}</span>
                        <h3 class="font-bold text-gray-800 mt-2 text-base">${item.name}</h3>
                    </div>
                    <div class="mt-4 flex justify-between items-center pt-2 border-t border-gray-50">
                        <span class="font-extrabold text-gray-900">${item.price.toFixed(3)} <small class="font-normal text-xs">ر.ع</small></span>
                        <span class="w-7 h-7 rounded-full bg-amber-100 text-amber-700 flex items-center justify-center text-sm font-bold">+</span>
                    </div>
                </div>
            `).join('');
        }

        // إدارة السلة
        function addToCart(id) {
            const item = appData.menu.find(i => i.id === id);
            const existing = currentCart.find(i => i.id === id);
            if (existing) {
                existing.qty += 1;
            } else {
                currentCart.push({ ...item, qty: 1 });
            }
            renderCart();
        }

        function changeQty(id, delta) {
            const item = currentCart.find(i => i.id === id);
            if (!item) return;
            item.qty += delta;
            if (item.qty <= 0) {
                currentCart = currentCart.filter(i => i.id !== id);
            }
            renderCart();
        }

        function clearCart() {
            currentCart = [];
            document.getElementById('customer-name').value = '';
            document.getElementById('customer-phone').value = '';
            renderCart();
        }

        function renderCart() {
            const container = document.getElementById('cart-items');
            if (currentCart.length === 0) {
                container.innerHTML = `<div class="text-center text-gray-400 py-12"><i class="fa-solid fa-mug-saucer text-4xl mb-2"></i><p class="text-sm">السلة فارغة، اختر الأصناف للبدء</p></div>`;
                document.getElementById('cart-total').innerText = "0.000 ر.ع";
                return;
            }

            let total = 0;
            container.innerHTML = currentCart.map(item => {
                const itemTotal = item.price * item.qty;
                total += itemTotal;
                return `
                    <div class="flex justify-between items-center bg-gray-50 p-2.5 rounded-lg border border-gray-200">
                        <div>
                            <h4 class="font-bold text-sm text-gray-800">${item.name}</h4>
                            <span class="text-xs text-gray-500">${item.price.toFixed(3)} ر.ع للواحد</span>
                        </div>
                        <div class="flex items-center gap-2">
                            <button onclick="changeQty(${item.id}, -1)" class="w-6 h-6 bg-gray-200 rounded text-gray-700 font-bold">-</button>
                            <span class="font-bold text-sm w-5 text-center">${item.qty}</span>
                            <button onclick="changeQty(${item.id}, 1)" class="w-6 h-6 bg-amber-500 text-white rounded font-bold">+</button>
                        </div>
                    </div>
                `;
            }).join('');

            document.getElementById('cart-total').innerText = total.toFixed(3) + " ر.ع";
        }

        // خيارات الطاولات
        function toggleTableSelect() {
            const type = document.getElementById('order-type').value;
            document.getElementById('table-select-wrapper').style.display = type === 'table' ? 'block' : 'none';
        }

        function renderTableSelects() {
            const posSelect = document.getElementById('order-table');
            const qrSelect = document.getElementById('qr-sim-table');
            let options = '';
            for (let i = 1; i <= appData.tablesCount; i++) {
                const isBusy = appData.activeTableOrders[i] ? ' (مشغولة)' : '';
                options += `<option value="${i}">طاولة ${i}${isBusy}</option>`;
            }
            posSelect.innerHTML = options;
            qrSelect.innerHTML = options;
        }

        // حفظ الطلب (تعليق للطاولة أو دفع فوري)
        function submitOrder(status) {
            if (currentCart.length === 0) return alert("يرجى اختيار أصناف أولاً!");

            const orderType = document.getElementById('order-type').value;
            const tableNum = document.getElementById('order-table').value;
            const customerName = document.getElementById('customer-name').value || "عميل";
            const total = currentCart.reduce((sum, i) => sum + (i.price * i.qty), 0);

            if (status === 'hold') {
                if (orderType !== 'table') return alert("تعليق الطلب متاح للطلبات المحلية (الطاولات) فقط.");
                
                // دمج مع طلب الطاولة الحالي إن وجد
                if (appData.activeTableOrders[tableNum]) {
                    currentCart.forEach(newItem => {
                        const existing = appData.activeTableOrders[tableNum].items.find(i => i.id === newItem.id);
                        if (existing) existing.qty += newItem.qty;
                        else appData.activeTableOrders[tableNum].items.push(newItem);
                    });
                } else {
                    appData.activeTableOrders[tableNum] = {
                        items: [...currentCart],
                        customer: customerName,
                        time: new Date().toLocaleTimeString('ar-OM', { hour: '2-digit', minute: '2-digit' }),
                        source: 'كاشير'
                    };
                }
                clearCart();
                saveData();
                alert(`تم تسجيل الطلب على طاولة رقم (${tableNum}) بنجاح!`);
            } else {
                // دفع وإغلاق
                const orderRecord = {
                    id: Date.now().toString().slice(-5),
                    type: orderType === 'table' ? `طاولة ${tableNum}` : 'سفري',
                    items: currentCart.map(i => `${i.name} (×${i.qty})`).join('، '),
                    time: new Date().toLocaleString('ar-OM'),
                    total: total
                };
                appData.completedOrders.unshift(orderRecord);

                if (orderType === 'table' && appData.activeTableOrders[tableNum]) {
                    delete appData.activeTableOrders[tableNum];
                }
                clearCart();
                saveData();
                alert("تم إتمام الدفع وحفظ الفاتورة بنجاح!");
            }
        }

        // خريطة الطاولات التفاعلية
        function renderTablesMap() {
            const grid = document.getElementById('tables-grid');
            const badge = document.getElementById('busy-tables-badge');
            const busyCount = Object.keys(appData.activeTableOrders).length;

            if (busyCount > 0) {
                badge.innerText = busyCount;
                badge.classList.remove('hidden');
            } else {
                badge.classList.add('hidden');
            }

            let html = '';
            for (let i = 1; i <= appData.tablesCount; i++) {
                const activeOrder = appData.activeTableOrders[i];
                if (activeOrder) {
                    const total = activeOrder.items.reduce((s, item) => s + (item.price * item.qty), 0);
                    html += `
                        <div class="bg-red-50 border-2 border-red-500 rounded-xl p-4 flex flex-col justify-between shadow-sm">
                            <div class="flex justify-between items-start">
                                <span class="font-extrabold text-lg text-red-700">طاولة ${i}</span>
                                <span class="bg-red-500 text-white text-xs px-2 py-0.5 rounded-full">${activeOrder.source}</span>
                            </div>
                            <div class="my-3 text-xs text-gray-700 space-y-1 max-h-24 overflow-y-auto">
                                ${activeOrder.items.map(item => `<div>• ${item.name} <b>(×${item.qty})</b></div>`).join('')}
                                ${activeOrder.note ? `<div class="text-amber-700 font-bold mt-1">ملاحظة: ${activeOrder.note}</div>` : ''}
                            </div>
                            <div class="border-t border-red-200 pt-2">
                                <div class="flex justify-between font-bold text-sm text-red-900 mb-2">
                                    <span>الحساب:</span>
                                    <span>${total.toFixed(3)} ر.ع</span>
                                </div>
                                <div class="grid grid-cols-2 gap-1">
                                    <button onclick="loadTableToPOS(${i})" class="bg-gray-800 text-white text-xs py-1.5 rounded font-bold">فتح بالكاشير</button>
                                    <button onclick="closeTableDirectly(${i})" class="bg-emerald-600 text-white text-xs py-1.5 rounded font-bold">دفع وإغلاق</button>
                                </div>
                            </div>
                        </div>
                    `;
                } else {
                    html += `
                        <div onclick="selectEmptyTable(${i})" class="bg-white border-2 border-emerald-400 hover:border-emerald-600 rounded-xl p-4 flex flex-col items-center justify-center h-44 cursor-pointer transition">
                            <i class="fa-solid fa-chair text-3xl text-emerald-500 mb-2"></i>
                            <span class="font-bold text-gray-800">طاولة ${i}</span>
                            <span class="text-xs text-emerald-600 mt-1">متاحة</span>
                        </div>
                    `;
                }
            }
            grid.innerHTML = html;
        }

        function selectEmptyTable(num) {
            switchTab('pos');
            document.getElementById('order-type').value = 'table';
            toggleTableSelect();
            document.getElementById('order-table').value = num;
        }

        function loadTableToPOS(num) {
            const order = appData.activeTableOrders[num];
            if (!order) return;
            currentCart = [...order.items];
            switchTab('pos');
            document.getElementById('order-type').value = 'table';
            toggleTableSelect();
            document.getElementById('order-table').value = num;
            renderCart();
        }

        function closeTableDirectly(num) {
            const order = appData.activeTableOrders[num];
            if (!order) return;
            const total = order.items.reduce((s, item) => s + (item.price * item.qty), 0);
            appData.completedOrders.unshift({
                id: Date.now().toString().slice(-5),
                type: `طاولة ${num} (${order.source})`,
                items: order.items.map(i => `${i.name} (×${i.qty})`).join('، '),
                time: new Date().toLocaleString('ar-OM'),
                total: total
            });
            delete appData.activeTableOrders[num];
            saveData();
        }

        // قسم الباركود (QR) ومحاكي طلب العميل الذاتي
        function renderQRSection() {
            const qrContainer = document.getElementById('qr-cards-container');
            const baseUrl = window.location.href.split('?')[0];
            let qrHtml = '';

            for (let i = 1; i <= appData.tablesCount; i++) {
                const tableUrl = `${baseUrl}?table=${i}`;
                const qrImgUrl = `https://api.qrserver.com/v1/create-qr-code/?size=150x150&data=${encodeURIComponent(tableUrl)}`;
                qrHtml += `
                    <div class="border rounded-lg p-3 text-center bg-gray-50 flex flex-col items-center">
                        <span class="font-bold text-xs text-amber-700 mb-1">${appData.cafeName}</span>
                        <img src="${qrImgUrl}" alt="QR Table ${i}" class="w-24 h-24 mb-2 border p-1 bg-white rounded">
                        <span class="font-extrabold text-sm text-gray-800">طاولة رقم ${i}</span>
                        <span class="text-[10px] text-gray-500">امسح للطلب المباشر</span>
                    </div>
                `;
            }
            qrContainer.innerHTML = qrHtml;

            // تحديث منيو جوال العميل
            const simMenu = document.getElementById('qr-sim-menu');
            simMenu.innerHTML = appData.menu.map(item => {
                const inSim = qrSimCart.find(i => i.id === item.id);
                const qty = inSim ? inSim.qty : 0;
                return `
                    <div class="bg-white p-2.5 rounded-lg shadow-sm border flex justify-between items-center">
                        <div>
                            <h4 class="font-bold text-xs text-gray-800">${item.name}</h4>
                            <span class="text-xs text-amber-600 font-bold">${item.price.toFixed(3)} ر.ع</span>
                        </div>
                        <div class="flex items-center gap-2">
                            ${qty > 0 ? `<button onclick="updateSimCart(${item.id}, -1)" class="w-6 h-6 bg-gray-200 rounded text-xs font-bold">-</button><span class="text-xs font-bold">${qty}</span>` : ''}
                            <button onclick="updateSimCart(${item.id}, 1)" class="px-2 py-1 bg-amber-600 text-white rounded text-xs font-bold">+ أضف</button>
                        </div>
                    </div>
                `;
            }).join('');

            const simTotal = qrSimCart.reduce((s, i) => s + (i.price * i.qty), 0);
            document.getElementById('qr-sim-total').innerText = simTotal.toFixed(3) + " ر.ع";
        }

        function updateSimCart(id, delta) {
            const item = appData.menu.find(i => i.id === id);
            const existing = qrSimCart.find(i => i.id === id);
            if (existing) {
                existing.qty += delta;
                if (existing.qty <= 0) qrSimCart = qrSimCart.filter(i => i.id !== id);
            } else if (delta > 0) {
                qrSimCart.push({ ...item, qty: 1 });
            }
            renderQRSection();
        }

        function submitCustomerQROrder() {
            if (qrSimCart.length === 0) return alert("يرجى اختيار مشروب أو صنف أولاً من قائمة الجوال!");
            const tableNum = document.getElementById('qr-sim-table').value;
            const note = document.getElementById('qr-sim-note').value;

            appData.activeTableOrders[tableNum] = {
                items: [...qrSimCart],
                note: note,
                time: new Date().toLocaleTimeString('ar-OM', { hour: '2-digit', minute: '2-digit' }),
                source: 'طلب باركود QR'
            };

            qrSimCart = [];
            document.getElementById('qr-sim-note').value = '';
            saveData();
            alert(`تم إرسال طلبك من الطاولة (${tableNum}) إلى الكاشير! تفقد شاشة الطاولات الآن.`);
            switchTab('tables');
        }

        // إرسال الفاتورة عبر الواتساب والطباعة
        function sendWhatsApp() {
            if (currentCart.length === 0) return alert("السلة فارغة!");
            const phone = document.getElementById('customer-phone').value.replace(/\D/g, '');
            if (!phone) return alert("يرجى إدخال رقم جوال العميل أولاً!");

            const fullPhone = phone.startsWith('968') ? phone : '968' + phone;
            const total = currentCart.reduce((s, i) => s + (i.price * i.qty), 0);
            let msg = `*مرحباً بك في ${appData.cafeName}* ☕\n`;
            msg += `تفاصيل فاتورتك:\n------------------\n`;
            currentCart.forEach(i => {
                msg += `• ${i.name} (×${i.qty}) = ${(i.price * i.qty).toFixed(3)} ر.ع\n`;
            });
            msg += `------------------\n*الإجمالي: ${total.toFixed(3)} ر.ع*\nشكراً لزيارتكم!`;

            window.open(`https://wa.me/${fullPhone}?text=${encodeURIComponent(msg)}`, '_blank');
        }

        function printReceipt() {
            if (currentCart.length === 0) return alert("السلة فارغة!");
            document.getElementById('print-cafe-name').innerText = appData.cafeName;
            const type = document.getElementById('order-type').value === 'table' ? 'طاولة ' + document.getElementById('order-table').value : 'طلب سفري';
            document.getElementById('print-order-info').innerText = `${type} | ${new Date().toLocaleString('ar-OM')}`;

            let total = 0;
            document.getElementById('print-items').innerHTML = currentCart.map(i => {
                const sub = i.price * i.qty;
                total += sub;
                return `<div class="flex justify-between"><span>${i.name} ×${i.qty}</span><span>${sub.toFixed(3)}</span></div>`;
            }).join('');
            document.getElementById('print-total').innerText = total.toFixed(3) + " ر.ع";
            window.print();
        }

        // التقارير والإعدادات
        function renderOrdersHistory() {
            const totalSales = appData.completedOrders.reduce((s, o) => s + o.total, 0);
            document.getElementById('stat-total-sales').innerText = totalSales.toFixed(3) + " ر.ع";
            document.getElementById('stat-total-orders').innerText = appData.completedOrders.length;
            document.getElementById('stat-active-tables').innerText = Object.keys(appData.activeTableOrders).length;

            const tbody = document.getElementById('orders-history-body');
            tbody.innerHTML = appData.completedOrders.map(o => `
                <tr class="hover:bg-gray-50">
                    <td class="p-3 font-mono font-bold">#${o.id}</td>
                    <td class="p-3">${o.type}</td>
                    <td class="p-3 text-gray-600">${o.items}</td>
                    <td class="p-3 text-xs text-gray-400">${o.time}</td>
                    <td class="p-3 font-bold text-emerald-700">${o.total.toFixed(3)} ر.ع</td>
                </tr>
            `).join('');
        }

        function clearSalesHistory() {
            if (confirm("هل أنت متأكد من تصفير سجل المبيعات بالكامل؟")) {
                appData.completedOrders = [];
                saveData();
            }
        }

        function saveCafeSettings() {
            const newName = document.getElementById('setting-cafe-name').value.trim();
            const newSub = document.getElementById('setting-cafe-sub').value.trim();
            const newCount = parseInt(document.getElementById('setting-tables-count').value) || 10;

            if (newName) appData.cafeName = newName;
            if (newSub) appData.cafeSub = newSub;
            appData.tablesCount = newCount;

            saveData();
            alert("تم تحديث اسم الكافيه والإعدادات بنجاح!");
        }

        function addNewMenuItem() {
            const name = document.getElementById('new-item-name').value.trim();
            const price = parseFloat(document.getElementById('new-item-price').value);
            const category = document.getElementById('new-item-cat').value;

            if (!name || isNaN(price)) return alert("يرجى إدخال اسم الصنف والسعر بشكل صحيح!");

            appData.menu.push({ id: Date.now(), name, price, category });
            document.getElementById('new-item-name').value = '';
            document.getElementById('new-item-price').value = '';
            saveData();
        }

        function deleteMenuItem(id) {
            appData.menu = appData.menu.filter(i => i.id !== id);
            saveData();
        }

        function renderSettingsMenu() {
            const list = document.getElementById('settings-menu-list');
            list.innerHTML = appData.menu.map(item => `
                <div class="py-2.5 flex justify-between items-center">
                    <div>
                        <span class="font-bold text-gray-800">${item.name}</span>
                        <span class="text-xs text-gray-500 mr-2">(${item.category})</span>
                    </div>
                    <div class="flex items-center gap-4">
                        <span class="font-bold text-amber-700">${item.price.toFixed(3)} ر.ع</span>
                        <button onclick="deleteMenuItem(${item.id})" class="text-red-500 hover:text-red-700 text-sm">
                            <i class="fa-solid fa-trash"></i> حذف
                        </button>
                    </div>
                </div>
            `).join('');
        }

        // تشغيل النظام عند فتح الصفحة
        window.onload = updateUI;
    </script>
</body>
</html>
