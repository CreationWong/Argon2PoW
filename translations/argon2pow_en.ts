<?xml version='1.0' encoding='utf-8'?>
<TS version="2.1" language="en" sourcelanguage="zh_CN">
    <context>
        <name>Application</name>
        <message>
            <location filename="../src/generator/main.cpp" line="17" />
            <source>Argon2 PoW 出题端</source>
            <translation>Argon2 PoW Challenge Generator</translation>
        </message>
        <message>
            <location filename="../src/solver/main.cpp" line="17" />
            <source>Argon2 PoW 解答端</source>
            <translation>Argon2 PoW Solver</translation>
        </message>
    </context>
    <context>
        <name>ChallengeWindow</name>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="57" />
            <source>Argon2 PoW 出题端</source>
            <translation>Argon2 PoW Challenge Generator</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="74" />
            <source>文件(&amp;F)</source>
            <translation>&amp;File</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="76" />
            <location filename="../src/generator/challengewindow.cpp" line="211" />
            <source>生成挑战码(&amp;G)</source>
            <translation>&amp;Generate challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="80" />
            <source>载入示例参数(&amp;E)</source>
            <translation>Load &amp;example parameters</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="83" />
            <source>导入并检查挑战码(&amp;I)</source>
            <translation>&amp;Import and check challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="91" />
            <location filename="../src/generator/challengewindow.cpp" line="232" />
            <source>复制挑战码(&amp;C)</source>
            <translation>&amp;Copy challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="95" />
            <source>退出(&amp;X)</source>
            <translation>E&amp;xit</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="101" />
            <source>帮助(&amp;H)</source>
            <translation>&amp;Help</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="102" />
            <source>关于(&amp;A)</source>
            <translation>&amp;About</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="113" />
            <source>设置计算要求，生成挑战码发给对方；收到解锁码后，可在下方校验。</source>
            <translation>Set the requirements and send the generated challenge. Verify the returned unlock code below.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="115" />
            <source>已有挑战码？导入并检查</source>
            <translation>Import an existing challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="120" />
            <source>要导入的挑战码</source>
            <translation>Challenge to import</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="121" />
            <source>粘贴 CHALLENGE:…，反查参数并校验解锁码</source>
            <translation>Paste CHALLENGE:… to inspect its parameters and verify an unlock code</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="123" />
            <source>导入并检查(&amp;I)</source>
            <translation>&amp;Import and check</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="129" />
            <source>检查格式与参数范围，回填下方设置；导入后可直接在第 3 步校验解锁码。</source>
            <translation>Check the format and parameter ranges, then fill in the settings below. Verify an unlock code in step 3.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="131" />
            <source>显示已有挑战码的导入与检查</source>
            <translation>Show challenge import and validation</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="131" />
            <source>收起已有挑战码的导入与检查</source>
            <translation>Hide challenge import and validation</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="136" />
            <source>1. 设置挑战参数</source>
            <translation>1. Set challenge parameters</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="163" />
            <source>每线程内存(&amp;M)：</source>
            <translation>&amp;Memory per thread:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="164" />
            <source>每个计算线程需要的内存，越大越占资源。</source>
            <translation>Memory required by each computing thread. Larger values use more resources.</translation>
        </message>
        <message numerus="yes">
            <location filename="../src/generator/challengewindow.cpp" line="170" />
            <location filename="../src/generator/challengewindow.cpp" line="172" />
            <source> 轮</source>
            <translation>
                <numerusform> round</numerusform>
                <numerusform> rounds</numerusform>
            </translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="174" />
            <source>计算轮数(&amp;T)：</source>
            <translation>Computing rounds (&amp;T):</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="175" />
            <source>单次尝试的计算轮数，越多耗时越长。</source>
            <translation>Rounds per attempt. More rounds take longer.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="184" />
            <source> 次</source>
            <translation> attempts</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="185" />
            <source>平均尝试次数(&amp;D)：</source>
            <translation>Average attempts (&amp;D):</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="186" />
            <source>即难度；实际可能更早或更晚找到答案。</source>
            <translation>The difficulty. An answer may be found earlier or later.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="193" />
            <source>自动生成（推荐）</source>
            <translation>Generate automatically (recommended)</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="194" />
            <location filename="../src/generator/challengewindow.cpp" line="197" />
            <source>自定义盐值</source>
            <translation>Custom salt</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="195" />
            <source>盐值用于区分挑战；自动模式下每次生成都会更新。</source>
            <translation>The salt distinguishes challenges. Automatic mode creates a new salt each time.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="198" />
            <source>8–128 个 ASCII 字符，不含空格和冒号</source>
            <translation>8–128 ASCII characters, without spaces or colons</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="203" />
            <source>盐值(&amp;S)：</source>
            <translation>&amp;Salt:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="209" />
            <source>载入示例</source>
            <translation>Load example</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="213" />
            <source>根据上方参数生成挑战码（Ctrl+G）；自动盐值每次都会改变。</source>
            <translation>Generate a challenge from these settings (Ctrl+G). Automatic mode changes the salt each time.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="220" />
            <source>2. 复制并发送挑战码</source>
            <translation>2. Copy and send the challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="223" />
            <source>参数已修改。请先点击“生成挑战码”，再复制或校验。</source>
            <translation>Settings have changed. Generate a new challenge before copying or verifying.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="229" />
            <source>已生成的挑战码</source>
            <translation>Generated challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="238" />
            <source>将完整挑战码发给对方，让对方在解答端粘贴并开始计算。</source>
            <translation>Send the complete challenge to the other person to paste into the solver and start computing.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="250" />
            <source>每线程内存：</source>
            <translation>Memory per thread:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="251" />
            <source>计算轮数：</source>
            <translation>Computing rounds:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="252" />
            <source>盐值：</source>
            <translation>Salt:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="253" />
            <source>平均尝试次数：</source>
            <translation>Average attempts:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="254" />
            <source>通过阈值：</source>
            <translation>Acceptance threshold:</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="255" />
            <source>显示当前挑战的技术参数</source>
            <translation>Show current challenge details</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="255" />
            <source>收起当前挑战的技术参数</source>
            <translation>Hide current challenge details</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="260" />
            <source>3. 校验对方的解锁码（可选）</source>
            <translation>3. Verify unlock code (optional)</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="265" />
            <source>待校验的解锁码</source>
            <translation>Unlock code to verify</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="266" />
            <source>粘贴对方返回的 UNBLOCK-… 解锁码</source>
            <translation>Paste the returned UNBLOCK-… unlock code</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="268" />
            <location filename="../src/generator/challengewindow.cpp" line="477" />
            <source>校验解锁码(&amp;V)</source>
            <translation>&amp;Verify unlock code</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="275" />
            <location filename="../src/generator/challengewindow.cpp" line="295" />
            <location filename="../src/generator/challengewindow.cpp" line="373" />
            <source>使用第 2 步显示的挑战码校验；粘贴解锁码后按 Enter 也可校验。</source>
            <translation>Verify against the challenge in step 2. Paste an unlock code and click Verify or press Enter.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="284" />
            <source>点击“导入并检查”或按 Enter，检查后才会替换当前挑战。</source>
            <translation>Click Import and check or press Enter. The active challenge changes only after validation.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="320" />
            <source>无法导入：%1 当前挑战未更改。</source>
            <translation>Cannot import: %1 The active challenge was not changed.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="333" />
            <source>格式与参数检查通过：内存 %1 KiB，%2 轮，难度 %3。已设为当前挑战，可在第 3 步校验解锁码。</source>
            <translation>Format and parameters are valid: %1 KiB, %2 rounds, difficulty %3. This is now the active challenge; verify an unlock code in step 3.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="336" />
            <source>已导入挑战码并还原参数；这不代表已有有效解锁码。</source>
            <translation>Challenge imported and parameters restored. This does not verify any unlock code.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="346" />
            <source>无法生成挑战码</source>
            <translation>Cannot generate challenge</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="375" />
            <source>挑战码已生成</source>
            <translation>Challenge generated</translation>
        </message>
        <message numerus="yes">
            <location filename="../src/generator/challengewindow.cpp" line="401" />
            <source>%n 轮</source>
            <translation>
                <numerusform>%n round</numerusform>
                <numerusform>%n rounds</numerusform>
            </translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="404" />
            <source>%1 次（约 1/%1 通过率）</source>
            <translation>%1 attempts (about a 1/%1 success rate per attempt)</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="406" />
            <source>%1（哈希前缀小于该值即通过）</source>
            <translation>%1 (the hash prefix must be smaller than this value)</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="417" />
            <location filename="../src/generator/challengewindow.cpp" line="449" />
            <source>参数已修改，请先重新生成挑战码。</source>
            <translation>Settings have changed. Generate a new challenge first.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="440" />
            <source>挑战码已复制到剪贴板</source>
            <translation>Challenge copied to the clipboard</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="454" />
            <source>无法校验</source>
            <translation>Cannot verify</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="458" />
            <source>请输入解锁码。</source>
            <translation>Enter an unlock code.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="472" />
            <source>正在校验…</source>
            <translation>Verifying…</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="473" />
            <source>正在本机校验，请稍候。参数越高，单次校验耗时越长。</source>
            <translation>Verifying locally. Higher parameter values take longer for each verification.</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="486" />
            <source>校验通过</source>
            <translation>Verification passed</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="486" />
            <source>校验未通过</source>
            <translation>Verification failed</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="494" />
            <source>关于 Argon2 PoW 出题端</source>
            <translation>About Argon2 PoW Challenge Generator</translation>
        </message>
        <message>
            <location filename="../src/generator/challengewindow.cpp" line="495" />
            <source>Argon2 PoW 出题端

生成兼容参考脚本的 Argon2id 挑战码。
挑战格式：CHALLENGE:内存KiB:时间成本:盐:难度
Argon2id，版本 19，并行度 1，32 字节哈希。</source>
            <translation>Argon2 PoW Challenge Generator

Generate Argon2id challenges compatible with the reference script.
Format: CHALLENGE:memoryKiB:timeCost:salt:difficulty
Argon2id, version 19, parallelism 1, 32-byte hash.</translation>
        </message>
    </context>
    <context>
        <name>LanguageMenu</name>
        <message>
            <location filename="../src/common/i18n.cpp" line="105" />
            <source>语言(&amp;L)</source>
            <translation>&amp;Language</translation>
        </message>
        <message>
            <location filename="../src/common/i18n.cpp" line="109" />
            <source>跟随系统</source>
            <translation>System default</translation>
        </message>
        <message>
            <location filename="../src/common/i18n.cpp" line="121" />
            <source>语言设置已保存，重新启动程序后生效。</source>
            <translation>Language preference saved. Restart the application to apply it.</translation>
        </message>
    </context>
    <context>
        <name>PoW</name>
        <message>
            <location filename="../src/common/pow.cpp" line="34" />
            <source>随机前缀格式不正确。</source>
            <translation>Invalid random prefix format.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="43" />
            <source>随机前缀只能包含大写字母和数字。</source>
            <translation>The random prefix can contain only uppercase letters and digits.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="100" />
            <source>请输入挑战码。</source>
            <translation>Enter a challenge.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="106" />
            <source>挑战码必须包含 5 段：CHALLENGE:内存:时间:盐:难度。</source>
            <translation>A challenge must have 5 fields: CHALLENGE:memory:time:salt:difficulty.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="117" />
            <source>挑战码应以 CHALLENGE 开头。</source>
            <translation>A challenge must start with CHALLENGE.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="124" />
            <source>内存参数必须在 %1 到 %2 KiB 之间。</source>
            <translation>Memory must be between %1 and %2 KiB.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="132" />
            <source>时间成本必须在 1 到 %1 之间。</source>
            <translation>Time cost must be between 1 and %1.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="145" />
            <source>难度必须是 1 到 %1 之间的整数。</source>
            <translation>Difficulty must be an integer between 1 and %1.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="161" />
            <source>盐长度必须在 8 到 128 个字符之间。</source>
            <translation>The salt must contain 8 to 128 characters.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="169" />
            <source>盐只能使用可打印 ASCII 字符，且不能包含冒号或空格。</source>
            <translation>The salt must use printable ASCII characters without colons or spaces.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="194" />
            <source>Argon2 计算失败：%1（错误码 %2）。</source>
            <translation>Argon2 calculation failed: %1 (error %2).</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="216" />
            <source>格式无效。解锁码应形如 UNBLOCK-XXXXXXXX-123（8 位大写字母/数字，加非零序号）。</source>
            <translation>Invalid format. Use UNBLOCK-XXXXXXXX-123 (8 uppercase letters/digits followed by a nonzero sequence number).</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="223" />
            <source>解锁码序号超出支持范围。</source>
            <translation>The unlock code sequence number is out of range.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="237" />
            <source>校验通过：哈希前缀 %1 &lt; 阈值 %2。</source>
            <translation>Verification passed: hash prefix %1 &lt; threshold %2.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="242" />
            <source>未达到难度要求：哈希前缀 %1 ≥ 阈值 %2。</source>
            <translation>Difficulty requirement not met: hash prefix %1 ≥ threshold %2.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="284" />
            <source>%1 秒</source>
            <translation>%1 seconds</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="324" />
            <source>挑战参数无效。</source>
            <translation>Invalid challenge parameters.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="329" />
            <source>难度阈值无效。</source>
            <translation>Invalid difficulty threshold.</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="410" />
            <source>无法创建计算线程：%1</source>
            <translation>Cannot create computing thread: %1</translation>
        </message>
        <message>
            <location filename="../src/common/pow.cpp" line="474" />
            <source>已搜索完 32 位序号空间，仍未找到符合条件的解。</source>
            <translation>The 32-bit sequence space was exhausted without finding an answer.</translation>
        </message>
    </context>
    <context>
        <name>SolverWindow</name>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="60" />
            <source>Argon2 PoW 解答端</source>
            <translation>Argon2 PoW Solver</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="83" />
            <source>文件(&amp;F)</source>
            <translation>&amp;File</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="85" />
            <source>载入示例挑战(&amp;E)</source>
            <translation>Load &amp;example challenge</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="89" />
            <source>退出(&amp;X)</source>
            <translation>E&amp;xit</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="93" />
            <source>计算(&amp;R)</source>
            <translation>&amp;Compute</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="94" />
            <source>开始 / 停止(&amp;S)</source>
            <translation>&amp;Start / Stop</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="100" />
            <source>帮助(&amp;H)</source>
            <translation>&amp;Help</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="101" />
            <source>关于(&amp;A)</source>
            <translation>&amp;About</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="112" />
            <source>粘贴挑战码并开始计算，找到解锁码后复制回原来的页面。计算在本机完成。</source>
            <translation>Paste a challenge and start computing. Copy the unlock code back to the original page when found. All computing is local.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="114" />
            <source>1. 粘贴挑战码</source>
            <translation>1. Paste a challenge</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="119" />
            <source>挑战码</source>
            <translation>Challenge</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="120" />
            <source>在此粘贴 CHALLENGE:… 挑战码</source>
            <translation>Paste a CHALLENGE:… code here</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="122" />
            <source>粘贴(&amp;P)</source>
            <translation>&amp;Paste</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="124" />
            <source>载入示例</source>
            <translation>Load example</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="130" />
            <location filename="../src/solver/solverwindow.cpp" line="266" />
            <location filename="../src/solver/solverwindow.cpp" line="326" />
            <source>粘贴后会自动检查格式。还没有挑战码？可先载入示例了解操作。</source>
            <translation>Pasted challenges are checked automatically. No challenge yet? Load the example to try the workflow.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="143" />
            <source>每线程内存：</source>
            <translation>Memory per thread:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="144" />
            <source>计算轮数：</source>
            <translation>Computing rounds:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="145" />
            <source>盐值：</source>
            <translation>Salt:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="146" />
            <source>平均尝试次数：</source>
            <translation>Average attempts:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="147" />
            <source>通过阈值：</source>
            <translation>Acceptance threshold:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="148" />
            <source>显示挑战的技术参数</source>
            <translation>Show challenge details</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="148" />
            <source>收起挑战的技术参数</source>
            <translation>Hide challenge details</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="153" />
            <source>2. 确认设置并开始</source>
            <translation>2. Confirm settings and start</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="156" />
            <source>计算线程(&amp;T)：</source>
            <translation>Computing &amp;threads:</translation>
        </message>
        <message numerus="yes">
            <location filename="../src/solver/solverwindow.cpp" line="161" />
            <location filename="../src/solver/solverwindow.cpp" line="287" />
            <source> 个</source>
            <translation>
                <numerusform> thread</numerusform>
                <numerusform> threads</numerusform>
            </translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="164" />
            <source>使用推荐值</source>
            <translation>Use recommended</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="167" />
            <location filename="../src/solver/solverwindow.cpp" line="661" />
            <source>开始解答(&amp;S)</source>
            <translation>&amp;Start solving</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="171" />
            <source>开始或停止计算（F5）。停止需要等待当前计算结束。</source>
            <translation>Start or stop computing (F5). Stopping waits for the current calculations to finish.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="177" />
            <location filename="../src/solver/solverwindow.cpp" line="374" />
            <location filename="../src/solver/solverwindow.cpp" line="398" />
            <source>内存占用：请先粘贴有效的挑战码。</source>
            <translation>Memory usage: paste a valid challenge first.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="180" />
            <location filename="../src/solver/solverwindow.cpp" line="376" />
            <source>将自动选择推荐线程数；需要降低电脑负载时，可减少线程。</source>
            <translation>A thread count is recommended automatically. Use fewer threads to reduce computer load.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="187" />
            <source>3. 复制解锁码</source>
            <translation>3. Copy the unlock code</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="192" />
            <source>计算得到的解锁码</source>
            <translation>Computed unlock code</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="195" />
            <source>复制解锁码(&amp;C)</source>
            <translation>&amp;Copy unlock code</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="205" />
            <source>计算状态</source>
            <translation>Computing status</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="212" />
            <source>计算活动状态</source>
            <translation>Computing activity</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="230" />
            <source>难度 ÷ 当前速率，是从现在起的平均等待估算；实际结束时间不确定。</source>
            <translation>Difficulty ÷ current speed estimates the average wait from now. The actual finish time is uncertain.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="231" />
            <source>已尝试：</source>
            <translation>Attempts:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="232" />
            <source>计算速度：</source>
            <translation>Computing speed:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="233" />
            <source>已用时间：</source>
            <translation>Elapsed time:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="234" />
            <source>平均等待估算：</source>
            <translation>Average wait estimate:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="241" />
            <source>找到答案的时间有随机性。尝试次数达到难度后仍可能继续计算；可随时停止。</source>
            <translation>Finding an answer is random. Computing may continue beyond the average attempt count. You can stop at any time.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="248" />
            <source>下一个候选：</source>
            <translation>Next candidate:</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="249" />
            <source>显示搜索细节</source>
            <translation>Show search details</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="249" />
            <source>收起搜索细节</source>
            <translation>Hide search details</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="267" />
            <source>正在检查挑战码…</source>
            <translation>Checking challenge…</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="311" />
            <source>等待挑战码</source>
            <translation>Waiting for a challenge</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="336" />
            <source>格式错误：%1</source>
            <translation>Format error: %1</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="339" />
            <location filename="../src/solver/solverwindow.cpp" line="341" />
            <source>挑战码格式错误</source>
            <translation>Invalid challenge format</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="358" />
            <source>格式正确。平均约需 %1 次尝试，每线程内存 %2。可点击“开始解答”。</source>
            <translation>Valid format. About %1 attempts on average, with %2 memory per thread. Click Start solving.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="364" />
            <source>挑战码解析成功</source>
            <translation>Challenge parsed successfully</translation>
        </message>
        <message numerus="yes">
            <location filename="../src/solver/solverwindow.cpp" line="389" />
            <source>%n 轮</source>
            <translation>
                <numerusform>%n round</numerusform>
                <numerusform>%n rounds</numerusform>
            </translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="391" />
            <location filename="../src/solver/solverwindow.cpp" line="522" />
            <location filename="../src/solver/solverwindow.cpp" line="555" />
            <source>%1 次</source>
            <translation>%1 attempts</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="410" />
            <source>预计内存占用：%1；本机可用约 %2（未含程序额外开销）。</source>
            <translation>Estimated memory: %1; about %2 available (excluding application overhead).</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="413" />
            <source>当前 %1 个线程高于推荐值 %2。更多线程不一定更快；若电脑卡顿，可停止后减少线程。</source>
            <translation>The selected %1 threads exceed the recommended %2. More threads may not be faster. Stop and reduce threads if the computer slows down.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="417" />
            <source>内存较紧，建议使用 1 个线程，避免系统换页。</source>
            <translation>Memory is limited. Use 1 thread to reduce paging.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="420" />
            <source>本机推荐 %1 个线程。每增加一个线程，都需要额外一份挑战内存。</source>
            <translation>Recommended: %1 threads. Each additional thread needs another allocation of the challenge memory.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="478" />
            <source>内存不足</source>
            <translation>Insufficient memory</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="479" />
            <source>按当前线程数预计需要 %1 内存，但本机当前可用约 %2。
请减少线程数，或关闭其他占用内存的程序后重试。</source>
            <translation>The selected threads require about %1 of memory, but only about %2 is available.
Use fewer threads or close other memory-intensive applications and retry.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="488" />
            <source>搜索中，找到后立即显示…</source>
            <translation>Searching; the unlock code will appear here…</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="490" />
            <source>正在计算，找到后会在这里显示解锁码。你可以随时点击“停止计算”。</source>
            <translation>Computing. The unlock code will appear here when found. You can click Stop computing at any time.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="491" />
            <source>正在解答当前挑战码。停止计算后可以更换挑战码或设置。</source>
            <translation>Solving the active challenge. Stop computing before changing the challenge or settings.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="512" />
            <source>正在停止…</source>
            <translation>Stopping…</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="513" />
            <source>正在停止，请等待当前一批计算结束。</source>
            <translation>Stopping; please wait for the current batch to finish.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="524" />
            <source>%1 次/秒</source>
            <translation>%1 attempts/s</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="533" />
            <source>按已尝试次数估算，累计找到答案的机会约 %1%（非完成进度）。</source>
            <translation>The estimated cumulative chance of finding an answer after these attempts is about %1% (not completion progress).</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="549" />
            <source>已找到有效解锁码，共计算 %1 次。复制后粘贴回原来的页面即可。</source>
            <translation>Valid unlock code found after %1 calculations. Copy it back to the original page.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="551" />
            <source>命中序号 %1；哈希前缀 %2，小于通过阈值 %3。</source>
            <translation>Sequence %1; hash prefix %2 is below the acceptance threshold %3.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="557" />
            <source>已找到有效答案，可以复制解锁码。</source>
            <translation>A valid answer was found. You can copy the unlock code.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="560" />
            <source>已命中：%1</source>
            <translation>Found: %1</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="561" />
            <source>已找到解锁码</source>
            <translation>Unlock code found</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="569" />
            <source>计算已停止，未找到解锁码。</source>
            <translation>Computing stopped without finding an unlock code.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="571" />
            <source>已停止，共尝试 %1 次；再次开始会使用新的随机前缀。</source>
            <translation>Stopped after %1 attempts. Starting again uses a new random prefix.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="575" />
            <source>已停止；重新开始会从新的随机前缀开始计算。</source>
            <translation>Stopped. Starting again uses a new random prefix.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="576" />
            <source>搜索已停止</source>
            <translation>Search stopped</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="581" />
            <source>搜索失败。</source>
            <translation>Search failed.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="585" />
            <source>计算失败，请检查提示后重试。</source>
            <translation>Computing failed. Review the error and retry.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="586" />
            <location filename="../src/solver/solverwindow.cpp" line="587" />
            <source>搜索失败</source>
            <translation>Search failed</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="606" />
            <source>已载入示例挑战</source>
            <translation>Example challenge loaded</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="614" />
            <source>找到的解会显示在这里</source>
            <translation>The unlock code will appear here when found</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="617" />
            <source>找到后点击“复制解锁码”，再粘贴回要求解答的页面。</source>
            <translation>When found, click Copy unlock code and paste it back into the page requesting the answer.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="618" />
            <source>0 次</source>
            <translation>0 attempts</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="619" />
            <source>0.0 次/秒</source>
            <translation>0.0 attempts/s</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="622" />
            <source>尚未开始计算。</source>
            <translation>Computing has not started.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="648" />
            <source>正在使用 %1 个线程搜索。停止操作会等待当前批次结束。</source>
            <translation>Searching with %1 threads. Stopping waits for the current batch to finish.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="652" />
            <source>当前挑战已解答。解锁码可在第 3 步复制。</source>
            <translation>This challenge has been solved. Copy the unlock code in step 3.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="653" />
            <source>计算已结束。可重新开始，或粘贴其他挑战码。</source>
            <translation>Computing has finished. Start again or paste a different challenge.</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="659" />
            <source>停止计算(&amp;S)</source>
            <translation>&amp;Stop computing</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="660" />
            <source>重新计算(&amp;S)</source>
            <translation>Compute again (&amp;S)</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="672" />
            <source>解锁码已复制到剪贴板</source>
            <translation>Unlock code copied to the clipboard</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="687" />
            <source>关于 Argon2 PoW 解答端</source>
            <translation>About Argon2 PoW Solver</translation>
        </message>
        <message>
            <location filename="../src/solver/solverwindow.cpp" line="688" />
            <source>Argon2 PoW 解答端

按参考脚本的规则搜索 UNBLOCK 解锁码。
Argon2id，版本 19，并行度 1，32 字节哈希。</source>
            <translation>Argon2 PoW Solver

Search for UNBLOCK codes using the reference script rules.
Argon2id, version 19, parallelism 1, 32-byte hash.</translation>
        </message>
    </context>
    <context>
        <name>Ui</name>
        <message>
            <location filename="../src/common/ui.cpp" line="21" />
            <source>界面语言：auto、zh_CN 或 en。</source>
            <translation>Interface language: auto, zh_CN or en.</translation>
        </message>
        <message>
            <location filename="../src/common/ui.cpp" line="22" />
            <source>载入示例参数。</source>
            <translation>Load example parameters.</translation>
        </message>
        <message>
            <location filename="../src/common/ui.cpp" line="23" />
            <source>自动开始解答。</source>
            <translation>Start solving automatically.</translation>
        </message>
        <message>
            <location filename="../src/common/ui.cpp" line="24" />
            <source>保存窗口截图并退出。</source>
            <translation>Save a window screenshot and exit.</translation>
        </message>
        <message>
            <location filename="../src/common/ui.cpp" line="25" />
            <source>截图前等待的毫秒数。</source>
            <translation>Milliseconds to wait before taking the screenshot.</translation>
        </message>
        <message>
            <location filename="../src/common/ui.cpp" line="26" />
            <source>要解答的挑战码。</source>
            <translation>The challenge to solve.</translation>
        </message>
    </context>
</TS>